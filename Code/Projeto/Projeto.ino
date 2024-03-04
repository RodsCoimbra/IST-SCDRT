#include "pid.h"
#include "command.h"

const int LED_PIN = 15;
const int DAC_RANGE = 4096;
int counter = 0;
const float vcc = 3.3;
const float adc_conv = 4095.0/vcc;
const float dutyCycle_conv = 4095.0/100.0;
const float offset_R_Lux = log10(225000)+0.8;
float K = 1.5;
// pid my_pid {3, K, K, K, 0.3, 3};
//(h+, K+, Ki, Kd, b ,Ti,
pid my_pid {5, 8, 3, 0, 0.3, 5};
float r {0.0};
float reference = r;


void setup() {// the setup function runs once
Serial.begin(115200);
analogReadResolution(12);//default is 10
analogWriteFreq(60000);//60KHz, about max
analogWriteRange(DAC_RANGE); //100% duty cycle
r = calculate_volt_lux(r);
}


void loop() {// the loop function runs cyclically
int read_adc, j;
float v_adc, Lux, total_adc, pwm;
int value;

//Média de 100 medições, para reduzir noise
for(j = 0, total_adc = 0; j < 50; j +=1){
read_adc = analogRead(A0); 
total_adc += read_adc;
delay(1);
}
read_adc = total_adc/50;

v_adc = calculate_Volt(read_adc); //Volt na entrada
Lux = calculate_Lux(read_adc);  //Lux na entrada (Alterar para v_adc se continuar a usar)

//Pid
pwm = my_pid.compute_control(r, v_adc);
value = calculate_adc_volt(pwm);
my_pid.housekeep(r, v_adc);
analogWrite(LED_PIN, value);


//format that Serial Plotter likes
Serial.print(value); Serial.print(" ");
Serial.print(Lux); Serial.print(" ");
Serial.print(reference); Serial.print(" ");
Serial.print(0); Serial.print(" ");
Serial.print(80); Serial.print(" ");
Serial.println();

//Commands
read_command();
}

float calculate_Volt(int read_adc){
  return read_adc/adc_conv;
}

float calculate_Lux(int read_adc){
float LDR_volt;
float LDR_resistance;
LDR_volt = read_adc/adc_conv;
LDR_resistance = (vcc * 10000.0)/LDR_volt - 10000.0;
return pow(10,(log10(LDR_resistance) - offset_R_Lux)/(-0.8));
}

float calculate_volt_lux(float lux){
  float resistance = pow(10, (-0.8 * log10(lux) + offset_R_Lux));
  return  (vcc * 10000.0)/(resistance + 10000.0); 
}

int calculate_adc_volt(float input_volt){
  int value = input_volt*adc_conv;
  if( value < 0 ) value = 0;
  if( value > 4095) value = 4095;
  return value;
}