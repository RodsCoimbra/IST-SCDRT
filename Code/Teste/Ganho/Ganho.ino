const int LED_PIN = 15;
const int DAC_RANGE = 4096;
const float vcc = 3.3;
const float adc_conv = 4095 / vcc; 
float m = -0.9;
float offset_R_Lux;
//x = gu -> x = lux, u = duty cycle
void setup() {  // the setup function runs once
  Serial.begin(115200);
  analogReadResolution(12);     //default is 10
  analogWriteFreq(60000);       //60KHz, about max
  analogWriteRange(DAC_RANGE);  //100% duty cycle
  analogWrite(LED_PIN, 0);
}


void loop() {  // the loop function runs cyclically
  int read_adc;
  float v_adc;
  float Lux;
  int i, j;
  float total_adc;
  float avg_lux;
  delay(5000);
  offset_R_Lux = log10(225000) - m;
  for (i = 0; i <= 4200; i += 300) {
    if (i > 4095) {
      i = 4095;
    }
    avg_lux = 0;
    total_adc = 0;
    analogWrite(LED_PIN, i);
    delay(1200);
    for (j = 0; j < 100; j += 1) {
      read_adc = analogRead(A0);  // read analog voltage
      total_adc += read_adc;
      delayMicroseconds(20);
    }
    avg_lux = calculate_Lux(total_adc/ 100.0);
    Serial.printf("%f, %f\n", calculate_Duty(i), avg_lux);
  }
  m+=0.01;
  if(m>-0.7){
    delay(10000000);
  }
}

float calculate_Volt(int read_adc) {
  return read_adc / adc_conv;
}

float calculate_Duty(int read_adc) {
  return read_adc / 4095.0;
}

float calculate_Lux(int read_adc) {
  float LDR_volt;
  float LDR_resistance;
  LDR_volt = read_adc / adc_conv;
  LDR_resistance = (vcc * 10000) / LDR_volt - 10000;
  return pow(10, (log10(LDR_resistance) - offset_R_Lux) / (m));
}