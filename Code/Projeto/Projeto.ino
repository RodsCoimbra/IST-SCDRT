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
float Pmax = 0.0158;
//pid my_pid {5, K, K, K, 1, 4.5};
//(h+, K+, Ki, Kd, b ,Ti,
 pid my_pid {5, 8, 3, 0, 0.3, 5};

float r {0.0};
float reference = r;
float DutyCycle = 0;
bool occupied = false, lux_flag = false, duty_flag = false, ignore_reference = false;
int read_adc;
short desk=1;
struct repeating_timer timer;
volatile bool timer_fired {false};

// last minute buffer

void setup() { // the setup function runs once
Serial.begin(115200);
analogReadResolution(12);//default is 10
analogWriteFreq(60000);//60KHz, about max
analogWriteRange(DAC_RANGE); //100% duty cycle
r = calculate_volt_lux(r);
//Gain measurement at the beginning of the program
G_inv = 1.0/Gain();
Serial.printf("The static gain of the system is %f\n", 1.0/G_inv);
add_repeating_timer_ms( -5, my_repeating_timer_callback, NULL, &timer);
}

void loop() {// the loop function runs cyclically
int j;
float v_adc, Lux, total_adc, pwm;
int value_adc;
//Média de 50 medições, para reduzir noise
unsigned long time;
if(timer_fired)
{
time = micros();
timer_fired = false;
for(j = 0, total_adc = 0; j < 20; j +=1){
read_adc = analogRead(A0); 
total_adc += read_adc;
}
read_adc = total_adc/20.0;

//Feedforward
if(!ignore_reference){
value_adc = (r*G_inv)*vcc;

//Pid
if(my_pid.get_feedback()){
v_adc = calculate_Volt(read_adc); //Volt na entrada
value_adc += my_pid.compute_control(r, v_adc, value_adc); //Volt
my_pid.housekeep(r, v_adc);
}
value_adc = constrain(calculate_adc_volt(value_adc), 0, 4095);
analogWrite(LED_PIN, value_adc);
DutyCycle = value_adc/dutyCycle_conv;
}

//format that Serial Plotter likes
//Lux = calculate_Lux(read_adc);
//Serial.print(value_adc); Serial.print(" ");
//Serial.print(Lux); Serial.print(" ");
//Serial.print(reference); Serial.print(" ");
//Serial.print(0); Serial.print(" ");
//Serial.print(80); Serial.print(" ");
//Serial.println();  
//Commands
read_command();
real_time_stream_of_data(time/1000);
}
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
analogWrite(LED_PIN, 819); //0.2 de duty cycle
delay(2500);
float y1 = calculate_Volt(analogRead(A0)); 
analogWrite(LED_PIN, 4095);
delay(2500);
float y2 = calculate_Volt(analogRead(A0)); 
analogWrite(LED_PIN, 0);
delay(2500);
return (y2-y1)/(1-0.2);
}


bool my_repeating_timer_callback (struct repeating_timer *t)
{
if(! timer_fired){
timer_fired = true;
}
return true;
}