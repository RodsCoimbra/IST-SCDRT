const int LED_PIN = 15;
const int DAC_RANGE = 4096;
const float vcc = 3.3;
const float adc_conv = 4095/vcc;
const float offset_R_Lux = log10(225000)+0.8;


void setup() {// the setup function runs once
Serial.begin(115200);
analogReadResolution(12);//default is 10
analogWriteFreq(60000);//60KHz, about max
analogWriteRange(DAC_RANGE); //100% duty cycle
}


void loop() {// the loop function runs cyclically
int read_adc;
float v_adc;
float Lux;
int i;
float tempo[1000][2], start, start_value;
analogWrite(LED_PIN, 0);

delay(5000);
start_value = calculate_Volt(analogRead(A0));
start = micros();
Serial.printf("%f %f",start, start_value);
Serial.println();
analogWrite(LED_PIN, 4095);
for(i = 0;i < 1000; i++){
  read_adc = analogRead(A0); // read analog voltage
  tempo[i][0] = micros();
  tempo[i][1] = calculate_Volt(read_adc);
  Serial.printf("%f %f\n",tempo[i][0], tempo[i][1]);
  delay(1);
}
delay(1000000000);
}

float calculate_Volt(int read_adc){
  return read_adc/adc_conv;
}

float calculate_duty_cycle(int input){
  return (input/4095)*100;
}