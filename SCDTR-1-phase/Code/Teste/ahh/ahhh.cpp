#include <hardware/flash.h> // for flash_get_unique_id
#include "mcp2515.h"
#include <iostream>
#include <iomanip>
#include <queue>

uint8_t this_pico_flash_id[8], node_address;
struct my_can_frame
{
    uint32_t can_id;                             /* 32 bit CAN_ID + EFF/RTR/ERR flags */
    uint8_t can_dlc;                             /* frame payload length in byte (0 .. 8) */
    uint8_t data[8] __attribute__((aligned(8))); /* frame payload data */
};
std::queue<my_can_frame> message_queue;
MCP2515::ERROR err;
const byte interruptPin{20};
volatile byte data_available{false};
MCP2515 can0{spi0, 17, 19, 16, 18, 10000000};

uint8_t my_id = 0;
bool connected = false;
bool pinging = false;
bool ack1_received = false;
bool ack2_received = false;
unsigned long ping_timeout = 15000; // Tempo limite para receber pings dos nós vizinhos (em milissegundos)
unsigned long last_ping_time;
uint8_t neighbor1_id = 0; // ID do primeiro nó vizinho
uint8_t neighbor2_id = 0; // ID do segundo nó vizinho

void read_interrupt(uint gpio, uint32_t events)
{
    data_available = true;
}

void connect()
{
    // Enviar uma mensagem de "wake up" com o ID do Pico como o ID da mensagem
    my_can_frame msg;
    msg.can_id = my_id; // Usar o ID do Pico
    msg.can_dlc = 8;
    msg.data[0] = 'W'; // Wake-up message
    msg.data[7] = node_address;

    for (int i = 1; i < 7; i++)
        msg.data[i] = ' ';

    message_queue.push(msg); // Adicionar a mensagem à fila de mensagens
}

void ping()
{
    // Enviar uma mensagem de ping com o ID do Pico como o ID da mensagem
    my_can_frame msg;
    msg.can_id = my_id; // Usar o ID do Pico
    msg.can_dlc = 8;
    msg.data[0] = 'P'; // Ping message
    msg.data[7] = node_address;

    for (int i = 1; i < 7; i++)
        msg.data[i] = ' ';

    message_queue.push(msg); // Adicionar a mensagem à fila de mensagens

    // Resetar o timer de timeout do ping
    last_ping_time = millis();
}

void setup()
{
    flash_get_unique_id(this_pico_flash_id); // obter o ID único do Pico
    node_address = this_pico_flash_id[7];    // usar o 5º byte do ID único como endereço do nó
    my_id = node_address;

    Serial.begin();                // inicializar a porta serial
    can0.reset();                  // redefinir o controlador CAN
    can0.setBitrate(CAN_1000KBPS); // definir a taxa de bits para 1Mbps
    can0.setNormalMode();          // definir o controlador para o modo normal
    gpio_set_irq_enabled_with_callback(interruptPin, GPIO_IRQ_EDGE_FALL, true, &read_interrupt);

    connect(); // Iniciar a conexão

    // Impressão de quando envia a mensagem de wake up
    Serial.println("Waiting for wake-up responses...");
}

void loop()
{
    if (!message_queue.empty()) // Se houver mensagens na fila
    {
        // Enviar a próxima mensagem na fila
        my_can_frame msg = message_queue.front();
        err = can0.sendMessage(reinterpret_cast<struct can_frame *>(&msg));
        message_queue.pop(); // Remover a mensagem da fila

        // Verificar erros na transmissão
        if (err != MCP2515::ERROR_OK)
        {
            Serial.print("Error sending message: ");
            Serial.println(err);
        }
    }

    if (data_available)
    {
        my_can_frame rx_msg;
        can0.readMessage(reinterpret_cast<struct can_frame *>(&rx_msg));

        // Verificar erros na recepção
        if (can0.checkError())
        {
            uint8_t errorFlags = can0.getErrorFlags();
            Serial.print("Error receiving message. Error flags: ");
            Serial.println(errorFlags, BIN);
        }

        if (rx_msg.data[0] == 'W' && !connected) // Se receber uma mensagem de "wake up" e ainda não estiver conectado
        {
            // Enviar um ack para o nó de origem
            my_can_frame ack_msg;
            ack_msg.can_id = rx_msg.can_id; // Enviar para o nó de origem
            ack_msg.can_dlc = 8;
            ack_msg.data[0] = 'A'; // Ack message
            ack_msg.data[7] = node_address;

            for (int i = 1; i < 7; i++)
                ack_msg.data[i] = ' ';

            message_queue.push(ack_msg); // Adicionar a mensagem de ack à fila de mensagens

            connected = true; // Marcar como conectado após receber o ack

            // Impressão de quando recebe a mensagem de wake up e envia o ack
            Serial.print("Received-wake up message from node ");
            Serial.print(rx_msg.can_id, HEX);
            Serial.println(" and sent ack.");
        }
        else if (rx_msg.data[0] == 'P' && pinging) // Se receber uma mensagem de ping e estiver na fase de ping
        {
            // Resetar o timer de timeout do ping
            last_ping_time = millis();
        }
        else if (rx_msg.data[0] == 'A' && connected) // Se receber um ack e estiver conectado
        {
            // Verificar se o ack é de um dos nós vizinhos
            uint8_t neighbor_id = rx_msg.data[7]; // ID do nó vizinho
            if (neighbor_id != node_address)      // Verificar se o ID não é o do próprio nó
            {
                if (!neighbor1_id || !neighbor2_id) // Se um dos IDs dos nós vizinhos ainda não foi recebido
                {
                    // Impressão do ID do nó vizinho que enviou o ack
                    Serial.print("Neighbor ID received: ");
                    Serial.println(neighbor_id, HEX);

                    // Atualizar o ID do vizinho correspondente
                    if (!neighbor1_id)
                        neighbor1_id = neighbor_id;
                    else
                        neighbor2_id = neighbor_id;

                    // Se ambos os IDs dos nós vizinhos foram recebidos
                    if (neighbor1_id && neighbor2_id)
                    {
                        Serial.print("Neighbor ID 1: ");
                        Serial.println(neighbor1_id, HEX);
                        Serial.print("Neighbor ID 2: ");
                        Serial.println(neighbor2_id, HEX);
                        ping();
                        pinging = true;
                    }
                }
            }
        }
    }

    if (!connected)
    {
        // Se não estiver conectado, enviar a mensagem de "wake up" novamente
        connect();
    }

    if (connected && pinging)
    {
        // Verificar se passou o tempo limite para receber pings dos nós vizinhos
        if (millis() - last_ping_time > ping_timeout)
        {
            // Impressão de que um nó foi desconectado devido ao timeout do ping
            Serial.println("Node disconnected due to ping timeout.");
            // Adicionar lógica para lidar com a desconexão
        }
    }
}