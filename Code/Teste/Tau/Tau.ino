const int LED_PIN = 15;
const int DAC_RANGE = 4096;
const float vcc = 3.3;
const float adc_conv = 4095 / vcc;
const float m = 0.89;
const float offset_R_Lux = log10(225000) - m;
int i = 0;

void setup() {  // the setup function runs once
  Serial.begin(115200);
  analogReadResolution(12);     //default is 10
  analogWriteFreq(60000);       //60KHz, about max
  analogWriteRange(DAC_RANGE);  //100% duty cycle
  analogWrite(LED_PIN, 0);
}


void loop() {  // the loop function runs cyclically
  int read_adc;
  float time, value;
  static bool turn_on = false;
  if (i < 4000) {
    value = calculate_Volt(analogRead(A0));
    time = micros();
    Serial.printf("%f %f\n", time, value);
    i++;
    if (!turn_on) {
      delay(8000);
      int j;
      for (j = 0; j < 100; j++) {
        value = calculate_Volt(analogRead(A0));
        time = micros();
        Serial.printf("%f %f\n", time, value);
      }
      time = micros();
      Serial.printf("%f %f\n", time, value);
      analogWrite(LED_PIN, 2047);
      turn_on = true;
      value = calculate_Volt(analogRead(A0));
      time = micros();
      Serial.printf("%f %f\n", time, value);
    }
  }
}

float calculate_Volt(int read_adc) {
  return read_adc / adc_conv;
}

float calculate_duty_cycle(int input) {
  return (input / 4095) * 100;
}
