const int LED_PIN = 15;
const int DAC_RANGE = 4096;
const float vcc = 3.3;
const float adc_conv = 4095.0 / vcc;
const float m = -0.89;
const float offset_R_Lux = log10(225000) - m;
int i = 0, j = 0;
int read_adc;
float value, total_adc, time_sample;
struct repeating_timer timer;
volatile bool timer_fired{ false };


int Valor_inicial = 0;
int Valor_final = 4095;


void setup() {  // the setup function runs once
  Serial.begin(115200);
  analogReadResolution(12);     //default is 10
  analogWriteFreq(60000);       //60KHz, about max
  analogWriteRange(DAC_RANGE);  //100% duty cycle
  add_repeating_timer_us(-800, my_repeating_timer_callback, NULL, &timer);
}


void loop() {  // the loop function runs cyclically
  static bool turn_on = false;
  if (timer_fired) {
    if (i < 4000) {
      time_sample = micros();
      for (j = 0, total_adc = 0; j < 10; j += 1) {
        total_adc += analogRead(A0);
      }
      read_adc = total_adc / 10.0;
      value = calculate_lux(read_adc);
      Serial.printf("%f %f\n", time_sample, value);
      i++;
      if (!turn_on) {
        analogWrite(LED_PIN, Valor_inicial);
        delay(10000);
        for (j = 0; j < 10; j++) {
          time_sample = micros();
          for (int k = 0, total_adc = 0; k < 10; k++) {
            total_adc += analogRead(A0);
          }
          read_adc = total_adc / 10.0;
          value = calculate_lux(read_adc);
          Serial.printf("%f %f\n", time_sample, value);
        }
        time_sample = micros();
        Serial.printf("%f %f\n", time_sample, value);
        analogWrite(LED_PIN, Valor_final);
        turn_on = true;
      }
    }
    timer_fired = false;
  }
}

float calculate_lux(int read_adc) {
  float volt = read_adc / adc_conv;
  float LDR_resistance = (vcc * 10000.0) / volt - 10000.0;
  return pow(10, (log10(LDR_resistance) - (offset_R_Lux)) / m);
}



float calculate_duty_cycle(int input) {
  return (input / 4095) * 100;
}

bool my_repeating_timer_callback(struct repeating_timer* t) {
  if (!timer_fired) {
    timer_fired = true;
  }
  return true;
}