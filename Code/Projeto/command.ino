#include "command.h"

void read_command(){
  char command;
  int lumminaire;
  if (Serial.available() > 0) {
    command = Serial.read();
    Serial.read(); 
    switch (command){
      case 'r':
        float temp_reference;
        lumminaire = Serial.parseInt();
        Serial.read();
        temp_reference = Serial.parseFloat();
        if (Serial.read() != '\n') {
          Serial.println("err");
          break;
        }
        reference = temp_reference;
        r = calculate_volt_lux(reference);
        Serial.println("ack");
        break;
      case 'd':
        float DutyC;
        lumminaire = Serial.parseInt();
        Serial.read();
        DutyC = Serial.parseFloat();
        if (Serial.read() != '\n' || DutyC > 100 || DutyC < 0) {
          Serial.println("err");
          break;
        }
        analogWrite(LED_PIN, DutyC*dutyCycle_conv);
        Serial.println("ack");
        delay(2000);
        break;
      case 'o':
        break;
      case 'a':
        break;
      case 'k':
        break;
      case 's':
        break;
      case 'S':
        break;
      case 'g':
        command = Serial.read();
        Serial.read(); 
        switch (command){
          case 'd':
            break;
          case 'l':
            break;
          case 'r':
            break;
          case 'o':
            break;
          case 'a':
            break;
          case 'k':
            break;
          case 'x':
            break;
          case 'p':
            break;
          case 't':
            break;
          case 'b':
            break;
          case 'e':
            break;
          case 'v':
            break;
          case 'f':
            break;
          default:
            Serial.println("err");
        }
        break;
      
      default:
        Serial.println("err");
    }
    return;
}
}
