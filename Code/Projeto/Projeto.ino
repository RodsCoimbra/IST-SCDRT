#include "pid.h"
#include "command.h"

const int LED_PIN = 15;
const int DAC_RANGE = 4096;
int counter = 0;
const float vcc = 3.3;
const float adc_conv = 4095/vcc;
const float offset_R_Lux = log10(225000)+0.8;
pid my_pid {0.01, 1, 0, 0.05 };
float r {0.0};
int DutyC;


void setup() {// the setup function runs once
Serial.begin(115200);
analogReadResolution(12);//default is 10
analogWriteFreq(60000);//60KHz, about max
analogWriteRange(DAC_RANGE); //100% duty cycle
}


void loop() {// the loop function runs cyclically
int read_adc;
float Lux;
if(Serial.available()){
  r = Serial.parseInt()
  Serial.flush()
  
}

read_adc = analogRead(A0); // read analog voltage
v_adc = calculate_Volt(read_adc);
Lux = calculate_Lux(read_adc);

//Pid
int pwm = (int) my_pid.compute_control(r, v_adc);
analogWrite(LED_PIN, pwm);
my_pid.housekeep(r, v_adc);


//format that Serial Plotter likes
Serial.print(0); Serial.print(" ");
Serial.print(Lux); Serial.print(" ");
Serial.printf("%d\n", pwm); Serial.print(" ");
Serial.print(1000); 
Serial.println();

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