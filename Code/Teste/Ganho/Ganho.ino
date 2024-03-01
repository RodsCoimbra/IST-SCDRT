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

int i,j;
float total_lux;
float avg_lux;
float avg_lux2;
delay(5000);
for(i = 0; i <= 4000; i +=200){
  avg_lux = 0;
  total_lux = 0;
  analogWrite(LED_PIN, i);
  delay(1000);
  for(j = 0; j < 100; j +=1){
read_adc = analogRead(A0); // read analog voltage

Lux = calculate_Lux(read_adc);  //Alterar para v_adc se continuar a usar

total_lux += Lux;
delay(5);
}
avg_lux = total_lux/100;
Serial.printf("Intensity -> %d, avg_lux -> %f ,", i, avg_lux);
}
delay(1000000);
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