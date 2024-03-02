#include "pid.h"
#include "command.h"

const int LED_PIN = 15;
const int DAC_RANGE = 4096;
int counter = 0;
const float vcc = 3.3;
const float adc_conv = 4095/vcc;
const float offset_R_Lux = log10(225000)+0.8;
pid my_pid {3, 0.8, 0.3, 3};
//pid my_pid {0.01, 1, 0, 0.05};
float r {40.0};
float reference = r;
int DutyC;


void setup() {// the setup function runs once
Serial.begin(115200);
analogReadResolution(12);//default is 10
analogWriteFreq(60000);//60KHz, about max
analogWriteRange(DAC_RANGE); //100% duty cycle
r = calculate_volt_lux(r);
}


void loop() {// the loop function runs cyclically
int read_adc;
float v_adc;
float Lux;
float total_adc;
int j;
for(j = 0, total_adc = 0; j < 50; j +=1){
read_adc = analogRead(A0); // read analog voltage
total_adc += read_adc;
delay(2);
}
read_adc = total_adc/50;
//read_adc = analogRead(A0); // read analog voltage
v_adc = calculate_Volt(read_adc);
Lux = calculate_Lux(read_adc);  //Alterar para v_adc se continuar a usar
char command;
int lumminaire;
//Pid
float pwm = (float) my_pid.compute_control(r, v_adc);
int value = calculate_Lux_volt(pwm);
my_pid.housekeep(r, v_adc);
analogWrite(LED_PIN, value);

//format that Serial Plotter likes
Serial.print(value); Serial.print(" ");
Serial.print(Lux); Serial.print(" ");
Serial.print(reference); Serial.print(" ");
Serial.print(0); Serial.print(" ");
Serial.print(80); Serial.print(" ");
Serial.println();


if (Serial.available() > 0) {
    command = Serial.read();
    Serial.read(); 
    if(command == 'r'){
      lumminaire = Serial.parseInt();
      Serial.read();
      reference = Serial.parseFloat();
      r = calculate_volt_lux(reference);
    }
}
delay(10);
}

float calculate_Volt(int read_adc){
  return read_adc/adc_conv;
}

float calculate_Lux(int read_adc){
float LDR_volt;
float LDR_resistance;
LDR_volt = read_adc/adc_conv;
LDR_resistance = (vcc * 10000)/LDR_volt - 10000;
return pow(10,(log10(LDR_resistance) - offset_R_Lux)/(-0.8));
}

float calculate_volt_lux(float lux){
  float resistance = pow(10, (-0.8 * log10(lux) + offset_R_Lux));
  return  (vcc * 10000)/(resistance + 10000); 
}


int calculate_Lux_volt(float read_adc){
float LDR_resistance;
LDR_resistance = (vcc * 10000)/read_adc - 10000;
int value = pow(10,(log10(LDR_resistance) - offset_R_Lux)/(-0.8));
if( value < 0 ) value = 0;
if( value > 4095) value = 4095;
return value;
}