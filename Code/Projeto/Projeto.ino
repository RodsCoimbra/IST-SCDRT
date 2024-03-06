#include "pid.h"
#include "command.h"

const int LED_PIN = 15;
const int DAC_RANGE = 4096;
const float vcc = 3.3;
const float adc_conv = 4095.0/vcc;
const float dutyCycle_conv = 4095.0/100.0;
const float m = -0.89;
const float offset_R_Lux = log10(225000)-m;
float G_inv;
float K = 1.5;
pid my_pid {5, K, K, K, 1, 4.5};
//(h+, K+, Ki, Kd, b ,Ti,
// pid my_pid {5, 8, 3, 0, 0.3, 5};

float r {40.0};
float reference = r;
float DutyCycle = 0;
bool occupied = false;


void setup() { // the setup function runs once
Serial.begin(115200);
analogReadResolution(12);//default is 10
analogWriteFreq(60000);//60KHz, about max
analogWriteRange(DAC_RANGE); //100% duty cycle
r = calculate_volt_lux(r);
//Gain measurement at the beginning of the program
G_inv = 1.0/Gain();
Serial.printf("The static gain of the system is %f\n", 1.0/G_inv);
}


void loop() {// the loop function runs cyclically
int read_adc, j;
float v_adc, Lux, total_adc, pwm;
int value_adc;

//Média de 50 medições, para reduzir noise
for(j = 0, total_adc = 0; j < 50; j +=1){
read_adc = analogRead(A0); 
total_adc += read_adc;
delay(1);
}
read_adc = total_adc/50.0;

//Feedforward
value_adc = (reference*G_inv)*4095;

//Pid
if(my_pid.get_feedback()){
v_adc = calculate_Volt(read_adc); //Volt na entrada
pwm = my_pid.compute_control(r, v_adc); //Volt
value_adc += calculate_adc_volt(pwm);
my_pid.housekeep(r, v_adc);
}
value_adc = constrain(value_adc, 0, 4095);
analogWrite(LED_PIN, value_adc);
DutyCycle = value_adc/dutyCycle_conv;


//format that Serial Plotter likes
Lux = calculate_Lux(read_adc);
Serial.print(value_adc); Serial.print(" ");
Serial.print(Lux); Serial.print(" ");
Serial.print(reference); Serial.print(" ");
Serial.print(0); Serial.print(" ");
Serial.print(80); Serial.print(" ");
Serial.println();  
//Commands
read_command();
delay(50);
}

float calculate_Volt(int read_adc){
  return read_adc/adc_conv;
}

float calculate_Lux(int read_adc){
float LDR_volt;
float LDR_resistance;
LDR_volt = read_adc/adc_conv;
LDR_resistance = (vcc * 10000.0)/LDR_volt - 10000.0;
return pow(10,(log10(LDR_resistance) - offset_R_Lux)/(m));
}

float calculate_volt_lux(float lux){
  float resistance = pow(10, (m * log10(lux) + offset_R_Lux));
  return  (vcc * 10000.0)/(resistance + 10000.0); 
}

int calculate_adc_volt(float input_volt){
  return input_volt*adc_conv;
}

float Gain(){
analogWrite(LED_PIN, 0);
delay(2500);
float y1 = calculate_Lux(analogRead(A0)); 
analogWrite(LED_PIN, 4095);
delay(2500);
float y2 = calculate_Lux(analogRead(A0)); 
analogWrite(LED_PIN, 0);
delay(2500);
return y2-y1;
}