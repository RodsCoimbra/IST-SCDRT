#include <hardware/flash.h> //for flash_get_unique_id
#include "mcp2515.h"
#include <queue>
#include <string>
#include <cstring>

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
int desk = 1, max_desk = 1;
struct repeating_timer timer;
volatile bool timer_fired{false};
uint8_t this_pico_flash_id[8], node_address;
MCP2515::ERROR err;
const byte interruptPin{20};
bool is_connected{false}, is_calibrated{false};
unsigned long connect_time, time_ack_start;
volatile byte data_available{false};
MCP2515 can0{spi0, 17, 19, 16, 18, 10000000};
int *desk_array = NULL;
float light_off, light_on;
bool wait_ack = false;
char char_comm_sent; 
int counter_ack = 0;
std::queue<can_frame> command_queue;
// the interrupt service routine
void read_interrupt(uint gpio, uint32_t events)
{
  data_available = true;
}

void setup()
{

  flash_get_unique_id(this_pico_flash_id);
  node_address = this_pico_flash_id[5];
  Serial.begin(115200);
  analogReadResolution(12);    // default is 10
  analogWriteFreq(60000);      // 60KHz, about max
  analogWriteRange(DAC_RANGE); // 100% duty cycle
  // ref_volt = lux_to_volt(ref);
  // add_repeating_timer_ms(-10, my_repeating_timer_callback, NULL, &timer);
  can0.reset();
  can0.setBitrate(CAN_1000KBPS);
  can0.setNormalMode();
  add_repeating_timer_ms(-100, my_repeating_timer_callback, NULL, &timer);
  gpio_set_irq_enabled_with_callback(interruptPin, GPIO_IRQ_EDGE_FALL, true, &read_interrupt);
  new_node_send();
  connect_time = millis();
}

void loop()
{
  unsigned long time_now = millis();
  if (!is_connected && time_now - connect_time > 1000)
  {
    // TO REMOVE -------------------------
    analogWrite(LED_PIN, 4095);
    delay_manual(3000);
    analogWrite(LED_PIN, 0);
    // TO REMOVE -------------------------

    Serial.printf("Conectei-me com desk = %d\n", desk);
    is_connected = true;
    max_desk = desk;
    new_calibration(); // Starts the calibration process
  }
  if (data_available || !command_queue.empty())
  {
    data_available = false;
    while (can0.checkReceive()) // Check if something has been received
    {
      can_frame canMsgRx;
      can0.readMessage(&canMsgRx);
      if (wait_ack) // If our node is waiting for an ack
      {
        if (canMsgRx.data[0] != 'A')
        {
          command_queue.push(canMsgRx);
        }
        else
        {
          if (canMsgRx.data[1] == char_comm_sent && char_msg_to_int(canMsgRx.data[2]) == desk)
          {
            counter_ack++;
            if (counter_ack == max_desk - 1)
            {
              wait_ack = false;
              counter_ack = 0;
            }
          }
          // while (!command_queue.empty())
          // {
          //     can_frame msg;
          //     msg = command_queue.front();
          //     Serial.printf("Final -> %s", msg.data);
          //   command_queue.pop();
          // }
        }
        data_available = false;
      }
      else
      {
        Serial.printf("Entrei no %c, next->%c\n", canMsgRx.data[0], canMsgRx.data[1]);
        switch (canMsgRx.data[0])
        {
        case 'C':
        {
          switch (canMsgRx.data[1])
          {
          case 'S':
          {
            Serial.printf("Entrei no S, desk = %d, msg = %d\n", desk, char_msg_to_int(canMsgRx.data[2]));
            if (char_msg_to_int(canMsgRx.data[2]) == desk)
            {
              counter_ack_calibration = 0;
              cross_gains();
            }
            else
            {
              analogWrite(LED_PIN, 0);
            }
          }
          break;
          case 'R':
          {
            // COMMENT light_on = adc_to_lux(digital_filter(50.0));
            calibration_msg(desk, 'A');
            // COMMENT desk_array[char_msg_to_int(canMsgRx.data[2])] = light_on - light_off;
          }
          break;
          case 'B': // Quando lerem o begin desligam as luzes
          {
            calibration_msg(desk, 'A');
            analogWrite(LED_PIN, 0);
          }
          break;
          case 'A': // Acknowledge
          {
            counter_ack_calibration++;
            if (!is_calibrated)
            {
              if (counter_ack_calibration == max_desk - 1)
              {
                delay_manual(2000);
                calibration_msg(desk, 'E');
                // COMMENT light_off = adc_to_lux(digital_filter(50.0));
              }
              else if (counter_ack_calibration == (max_desk - 1) * 2) // Quando lerem a external mandam mais um ack por desk, logo entra com o dobro dos acks
              {
                calibration_msg(1, 'S');
                is_calibrated = true;
              }
            }
            else
            {
              if (counter_ack_calibration == max_desk - 1)
              {
                if (desk != max_desk)
                {
                  analogWrite(LED_PIN, 0);
                  calibration_msg(desk + 1, 'S');
                }
                else
                {
                  calibration_msg(0, 'F');
                  analogWrite(LED_PIN, 0);
                }
              }
            }
          }
          break;
          case 'E':
          {
            // COMMENT light_off = adc_to_lux(digital_filter(50.0));
            calibration_msg(desk, 'A');
          }
          break;
          case 'F': // Acknowledge
          {
            Serial.printf("Calibration Finished through message\n");
          }
          break;
          default:
            Serial.printf("ERROR DURING CALIBRATION %c\n", canMsgRx.data[1]);
            break;
          }
        }
        break;
        case 'W':
        {
          switch (canMsgRx.data[1])
          {
          case 'N':
          {
            wake_up_receive();
          }
          break;
          case 'R':
          {
            if (!is_connected)
            {
              desk = char_msg_to_int(canMsgRx.data[2]);
              new_node_ack();
            }
          }
          break;
          case 'A':
          {
            max_desk = char_msg_to_int(canMsgRx.data[2]);
          }
          break;
          default:
            Serial.printf("ERROR DURING WAKE UP %c\n", canMsgRx.data[1]);
            break;
          }
        }
        break;
        default:
          break;
        }
      }
    }
  }
  // if (timer_fired)
  // {
  //   Serial.printf("Connected %d\n", desk);
  //   timer_fired = false;
  // }
}

// Mensagem -> "W N" (WAKE NEW) para registar
void new_node_send()
{
  struct can_frame canMsgTx;
  canMsgTx.can_id = node_address;
  canMsgTx.can_dlc = 8;
  canMsgTx.data[0] = 'W';
  canMsgTx.data[1] = 'N';
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

// Mensagem -> "W A {desk_number}" (Wake ACK) para informar os outros nós da recepção da mensagem
void new_node_ack()
{
  struct can_frame canMsgTx;
  canMsgTx.can_id = node_address;
  canMsgTx.can_dlc = 8;
  canMsgTx.data[0] = 'W';
  canMsgTx.data[1] = 'A';
  canMsgTx.data[2] = int_to_char_msg(desk); // Dizer o ID dele que será o novo MAX
  for (int i = 3; i < 8; i++)
  {
    canMsgTx.data[i] = ' ';
  }
  err = can0.sendMessage(&canMsgTx);
  if (err != MCP2515::ERROR_OK)
  {
    Serial.printf("Error sending message: %s\n", err);
  }
}

// Mensagem -> "W R {desk_number}" para informar o novo nó do seu número da desk
void wake_up_receive()
{
  if (desk_array != NULL)
  {
    free(desk_array);
  }
  struct can_frame canMsgTx;
  canMsgTx.can_id = node_address;
  canMsgTx.can_dlc = 8;
  canMsgTx.data[0] = 'W';
  canMsgTx.data[1] = 'R';
  canMsgTx.data[2] = int_to_char_msg(max_desk + 1);
  for (int i = 3; i < 8; i++)
  {
    canMsgTx.data[i] = ' ';
  }
  err = can0.sendMessage(&canMsgTx);
  if (err != MCP2515::ERROR_OK)
  {
    Serial.printf("Error sending message: %s\n", err);
  }
}

bool my_repeating_timer_callback(struct repeating_timer *t)
{
  if (!timer_fired)
  {
    timer_fired = true;
  }
  return true;
}

// Mensagem -> "C A/B/E/F/R/S {desk_number}" (Calibration Ack/Beginning/External/Finished/Read/Start) para informar que nó comecará a calibração
void calibration_msg(int desk, char type)
{
  struct can_frame canMsgTx;
  canMsgTx.can_id = node_address;
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
}

void new_calibration()
{
  desk_array = (int *)malloc(max_desk * sizeof(int)); // array of desks
  if (max_desk == 1)
  {
    Gain();
    is_calibrated = true;
  }
  else
  {
    Serial.printf("B msg\n");
    calibration_msg(desk, 'B');
    counter_ack_calibration = 0;
    analogWrite(LED_PIN, 0);
  }
}

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

void resend_last_msg()
{
  // TODO
}


//TODO acks/ char_msg_command é o ultimo comando enviado 