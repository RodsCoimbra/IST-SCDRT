#include "command.h"

void read_command(){
  char command;
  int lumminaire;
  if (Serial.available() > 0) {
    command = Serial.read();
    Serial.read(); 
    switch (command){
      case 'r':
        {
          float temp_reference;
          lumminaire = Serial.parseInt();
          Serial.read();
          temp_reference = Serial.parseFloat();
          if (Serial.read() != '\n') 
          {
            Serial.println("err");
            break;
          }
          reference = temp_reference;
          r = calculate_volt_lux(reference);
          Serial.println("ack");
        }
        break;
      case 'd':
        {
          float DutyC;
          lumminaire = Serial.parseInt();
          Serial.read();
          DutyC = Serial.parseFloat();
          if (Serial.read() != '\n' || DutyC > 100 || DutyC < 0) 
          {
            Serial.println("err");
            break;
          }
          analogWrite(LED_PIN, DutyC*dutyCycle_conv);
          Serial.println("ack");
          delay(2000);
        }
        break;
      case 'o':
        {
          int temp_occupied;
          lumminaire = Serial.parseInt();
          Serial.read();
          temp_occupied = Serial.parseInt();
          if (Serial.read() != '\n' || (temp_occupied != 1 && temp_occupied != 0)) 
          {
            Serial.println("err");
            break;
          }
          occupied = temp_occupied; 
          Serial.println("ack");
        }
        break;
      case 'a':
      {
        int temp_antiwindup;
        lumminaire = Serial.parseInt();
        Serial.read();
        temp_antiwindup = Serial.parseInt();
        if (Serial.read() != '\n' || (temp_antiwindup != 1 && temp_antiwindup != 0)) 
          {
            Serial.println("err");
            break;
          }
          my_pid.set_antiwindup(temp_antiwindup);
          Serial.println("ack");
      }
        break;
      case 'k':
      {
        int temp_feedback;
        lumminaire = Serial.parseInt();
        Serial.read();
        temp_feedback = Serial.parseInt();
        if (Serial.read() != '\n' || (temp_feedback != 1 && temp_feedback != 0)) 
          {
            Serial.println("err");
            break;
          }
          my_pid.set_feedback(temp_feedback);
          Serial.println("ack");
      }
        break;
      case 's':
        break;
      case 'S':
        break;
      case 'g':
        command = Serial.read();
        Serial.read(); 
        lumminaire = Serial.parseInt();
        Serial.read();
        switch (command){
          case 'd':
            Serial.printf("d %d %f\n", lumminaire, DutyCycle);
            break;
          case 'l':
            {
            int total_adc, read_adc, j;
            float Lux;
            for(j = 0, total_adc = 0; j < 20; j +=1)
            {
              read_adc = analogRead(A0); 
              total_adc += read_adc;
            }
            read_adc = total_adc/20.0;
            Lux = calculate_Lux(read_adc); 
            Serial.printf("l %d %f\n", lumminaire, Lux);
            }
            break;
          case 'r':
            Serial.printf("r %d %f\n", lumminaire, reference);
            break;
          case 'o':
            Serial.printf("o %d %d\n", lumminaire, occupied);
            break;
          case 'a':
            Serial.printf("a %d %d\n", lumminaire, my_pid.get_antiwindup());
            break;
          case 'k':
            Serial.printf("k %d %d\n", lumminaire, my_pid.get_feedback());
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
