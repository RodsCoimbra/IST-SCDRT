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
          ignore_reference = false;
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
          DutyCycle = DutyC;
          analogWrite(LED_PIN, DutyC*dutyCycle_conv);
          Serial.println("ack");
          ignore_reference = true;
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
        {
          char flag = Serial.read();
          Serial.read();
          lumminaire = Serial.parseInt();
          if (Serial.read() != '\n') 
          {
            Serial.println("err");
            return;
          }
          switch (flag) {
          case 'l':
            lux_flag = true;
            break;
          case 'd':
            duty_flag = true;
            break;
          default:
            Serial.println("err");
          }
        }
        break;
      case 'S':
        {
          char flag = Serial.read();
          Serial.read();
          lumminaire = Serial.parseInt();
          if (Serial.read() != '\n') 
          {
            Serial.println("err");
            return;
          }
          switch (flag) {
          case 'l':
            lux_flag = false;
            break;
          case 'd':
            duty_flag = false;
            break;
          default:
            Serial.println("err");
          }
        }
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
            {
            float Lux = calculate_Lux(read_adc);
            Lux = max(0,Lux - (DutyCycle/(100*G_inv)));
            Serial.printf("x %d %f\n", lumminaire, Lux);
            }
            break;
          case 'p':
            {
            float power = Pmax * DutyCycle/100.0;
            Serial.printf("p %d %f\n", lumminaire, power);
            }
            break;
          case 't':
            {
              unsigned long final_time = millis();
              Serial.printf("t %d %ld\n", lumminaire, final_time/1000);
            }
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

void real_time_stream_of_data(unsigned long time){
  if(lux_flag){
    float lux = calculate_Lux(read_adc);
    Serial.printf("s l %d %f %ld \n", desk,lux,time);
  }
  if(duty_flag){
    Serial.printf("s d %d %f %ld \n", desk, DutyCycle,time);
  }

}
