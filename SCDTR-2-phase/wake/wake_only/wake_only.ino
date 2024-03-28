#include <hardware/flash.h> //for flash_get_unique_id
#include "mcp2515.h"
#include <queue> // std::queue
#include <set>   // std::set

// Lumminaire
const int LED_PIN = 15;
const int DAC_RANGE = 4096;
// const float VCC = 3.3;
// const float adc_conv = 4095.0 / VCC;
// const float dutyCycle_conv = 4095.0 / 100.0;
// pid my_pid{0.01, 0.5, 1, 0.1, 0.05};
// pid my_pid(float _h, float _K, float b_,float Ti_, float Tt_, float Td_, float N_)
//  pid my_pid {5, 8, 3, 0, 0.3, 5};

// lumminaire my_desk{-0.89, log10(225000) - (-0.89), 0.0158, 1};
// float ref{0.0};
// float ref_volt;
// float read_adc;
// bool debbuging = false;
int desk = 1, num_desks = 1;
uint8_t this_pico_flash_id[8], node_address;

// TIMERS AND INTERRUPTS
struct repeating_timer timer;
volatile bool timer_fired{false};
const byte interruptPin{20};
volatile byte data_available{false};

void read_interrupt(uint gpio, uint32_t events)
{
    data_available = true;
}

bool my_repeating_timer_callback(struct repeating_timer *t)
{
    if (!timer_fired)
    {
        timer_fired = true;
    }
    return true;
}

// CONNECTION
unsigned long connect_time;
std::set<int> desks_connected; // TODO
bool is_connected{false};
int time_to_connect{500};

// MESSAGES
MCP2515::ERROR err;
MCP2515 can0{spi0, 17, 19, 16, 18, 10000000};
bool wait_ack = false;
std::queue<can_frame> command_queue;
can_frame last_msg_sent;
unsigned short last_msg_counter;

// CALIBRATION

// Start
void setup()
{
    Serial.begin(115200);
    analogReadResolution(12);    // default is 10
    analogWriteFreq(60000);      // 60KHz, about max
    analogWriteRange(DAC_RANGE); // 100% duty cycle
    add_repeating_timer_ms(-10, my_repeating_timer_callback, NULL, &timer);
    // ref_volt = lux_to_volt(ref);  //TODO : passar para a classe e depois fazer lá isto
    flash_get_unique_id(this_pico_flash_id);
    node_address = this_pico_flash_id[5];
    can0.reset();
    can0.setBitrate(CAN_1000KBPS);
    can0.setNormalMode();
    gpio_set_irq_enabled_with_callback(interruptPin, GPIO_IRQ_EDGE_FALL, true, &read_interrupt);

    // Connection timers
    randomSeed(node_address);
    time_to_connect += (random(21) * 100); // To ensure that the nodes don't connect at the same time
    connect_time = millis();
}

void loop()
{
    static int timer_new_node = 0;
    unsigned long time_now = millis();
    if (!is_connected)
    {
        if (time_now - connect_time > time_to_connect)
        {
            is_connected = true;
            desk = find_desk();
            // Serial.printf("Conectei-me com desk = %d e time_connect = %d ms\n", desk, time_to_connect);
            num_desks = desks_connected.size() + 1;
            connection_msg('A');
            // TODO Confirmar mensagens de W N antes de correr o new_calibration
        }
        else
        {
            if (time_now - timer_new_node > 200) // Send a new connection message every 200ms
            {
                connection_msg('N');
                timer_new_node = millis();
            }
        }
    }
    if ((data_available || !command_queue.empty()))
    {
        data_available = false;
        while (can0.checkReceive()) // Check if something has been received
        {
            can_frame canMsgRx;
            can0.readMessage(&canMsgRx);
            // TODO Ainda meter este wait ack a funcionar
            if (wait_ack) // If our node is waiting for an ack
            {
                if (canMsgRx.data[0] != 'A')
                {
                    command_queue.push(canMsgRx);
                }
                else
                {
                    // while (!command_queue.empty())
                    // {
                    //     can_frame msg;
                    //     msg = command_queue.front();
                    //     Serial.printf("Final -> %s", msg.data);
                    //   command_queue.pop();
                    // }
                }
            }
            else
            {
                switch (canMsgRx.data[0])
                {
                case 'W':
                {
                    msg_received_connection(canMsgRx);
                }
                break;
                default:
                    break;
                }
            }
        }
    }

    if (Serial.available() > 0) // TO DELETE
    {
        char c = Serial.read();
        if (is_connected)
        {
            Serial.printf("Desk Number: %d\n", desk);
            Serial.printf("Conectadas a mim:");
            for (const int &elem : desks_connected)
            {
                Serial.printf(" %d,", elem);
            }
            Serial.printf("\n Time to connect -> %d\n", time_to_connect);
        }
        else
        {
            Serial.printf("Not connected yet\n");
        }
    }
}

/************CONNECTION FUNCTIONS********************/

// Message -> "W A/N/R {desk_number}" (Wake Ack/New/Received)
void connection_msg(char type)
{
    struct can_frame canMsgTx;
    canMsgTx.can_id = desk;
    canMsgTx.can_dlc = 8;
    canMsgTx.data[0] = 'W';
    canMsgTx.data[1] = type;
    for (int i = 2; i < 8; i++)
    {
        canMsgTx.data[i] = ' ';
    }
    err = can0.sendMessage(&canMsgTx);
    if (err != MCP2515::ERROR_OK)
    {
        Serial.printf("Error sending message: %s\n", err);
    }
}

int find_desk()
{
    int i = 1;
    while (desks_connected.find(i) != desks_connected.end())
    {
        i++;
    }
    return i;
}

/************CALIBRATION FUNCTIONS********************/
// Message -> "C A/B/E/F/R/S {desk_number}" (Calibration Ack/Beginning/External/Finished/Read/Start)
void calibration_msg(int desk, char type)
{
    struct can_frame canMsgTx;
    canMsgTx.can_id = desk;
    canMsgTx.can_dlc = 8;
    canMsgTx.data[0] = 'C';
    canMsgTx.data[1] = type;
    canMsgTx.data[2] = int_to_char_msg(desk);
    for (int i = 3; i < 8; i++)
    {
        canMsgTx.data[i] = ' ';
    }
    err = can0.sendMessage(&canMsgTx);
    if (err != MCP2515::ERROR_OK)
    {
        Serial.printf("Error sending message: %s\n", err);
    }
    wait_ack = true;
    last_msg_sent = canMsgTx;
}

// void new_calibration()
// {
//     desk_array = (int *)malloc(num_desks * sizeof(int)); // array of desks
//     if (num_desks == 1)
//     {
//         Gain();
//         is_calibrated = true;
//     }
//     else
//     {
//         Serial.printf("B msg\n");
//         calibration_msg(desk, 'B');
//         counter_ack_calibration = 0; // TODO change this
//         analogWrite(LED_PIN, 0);
//     }
// }

void cross_gains()
{
    analogWrite(LED_PIN, 4095);
    delay_manual(2500);
    calibration_msg(desk, 'R');
    // COMMENT light_on =adc_to_lux(digital_filter(50.0));
    // COMMENT desk_array[desk] = light_on - light_off;
}

void Gain()
{
    Serial.println("Calibrating the gain of the system:");
    analogWrite(LED_PIN, 0);
    delay_manual(1000); // COMMENT MUDAR OS TRÊS PARA 2500
    // COMMENT light_off = adc_to_lux(digital_filter(50.0));
    analogWrite(LED_PIN, 4095);
    delay_manual(1000);
    // COMMENT light_on = adc_to_lux(digital_filter(50.0));
    analogWrite(LED_PIN, 0);
    delay_manual(1000);
    // float Gain = (light_on - light_off);
    Serial.printf("Calibration Finished\n");
}
/************UTILS********************/

void delay_manual(unsigned long delay)
{
    unsigned long delay_start = millis();
    unsigned long delay_end = millis();
    while (delay_end - delay_start < delay)
    {
        delay_end = millis();
    }
}

float digital_filter(float value)
{
    float total_adc;
    int j;
    for (j = 0, total_adc = 0; j < value; j += 1)
    {
        total_adc += analogRead(A0);
    }
    return total_adc / value;
}

int char_msg_to_int(char msg)
{
    return msg - '0';
}

char int_to_char_msg(int msg)
{
    return msg + '0';
}

void ack_msg(can_frame orig_msg)
{
    struct can_frame canMsgTx;
    canMsgTx.can_id = desk;
    canMsgTx.can_dlc = 8;
    canMsgTx.data[0] = 'A';
    for (int i = 0; i < 6; i++)
    {
        canMsgTx.data[i + 1] = orig_msg.data[i];
    }
    err = can0.sendMessage(&canMsgTx);
    wait_ack = true;
    last_msg_sent = canMsgTx;
    if (err != MCP2515::ERROR_OK)
    {
        Serial.printf("Error sending message: %s\n", err);
    }
}

bool confirm_msg(can_frame ack_msg)
{
    for (int i = 0; i < 6; i++)
    {
        if (ack_msg.data[i + 1] != last_msg_sent.data[i])
        {
            return false;
        }
    }
    return true;
}

void resend_last_msg()
{
    last_msg_counter++;
    if (last_msg_counter == 3)
    {
        last_msg_counter = 0;
        num_desks--;
        // TODO remove desk from desks_connected
        return;
    }
    err = can0.sendMessage(&last_msg_sent);
    if (err != MCP2515::ERROR_OK)
    {
        Serial.printf("Error sending message: %s\n", err);
    }
}

void msg_received_connection(can_frame canMsgRx)
{
    switch (canMsgRx.data[1])
    {
    case 'N':
    {
        if (is_connected)
        {
            connection_msg('R');
        }
    }
    break;
    case 'R':
    {
        if (!is_connected)
        {
            desks_connected.insert(canMsgRx.can_id);
        }
    }
    break;
    case 'A':
    {
        if (is_connected)
        {
            desks_connected.insert(canMsgRx.can_id);
        }
    }
    break;
    default:
        Serial.printf("ERROR DURING WAKE UP. Message W %c received.\n", canMsgRx.data[1]);
        break;
    }
}

// TODO acks/ char_msg_command é o ultimo comando enviado