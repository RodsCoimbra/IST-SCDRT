#include "pid.h"
#include "command.h"
#include "lumminaire.h"

const int LED_PIN = 15;
const int DAC_RANGE = 4096;
const float VCC = 3.3;
const float adc_conv = 4095.0 / VCC;
const float dutyCycle_conv = 4095.0 / 100.0;
pid my_pid{0.01, 2000, 0.75, 0.35, 0.5};
//(h, K, b ,Ti, TT
// pid my_pid {5, 8, 3, 0, 0.3, 5};

lumminaire my_desk{ -0.89, log10(225000) - ( -0.89), 0.0158, 1 };
// system my_desk{float _m, float _offset_R_Lux, float _Pmax, unsigned short _desk_number}

float r{ 0.0 };
float reference = r;
float read_adc;
bool debbuging = false;
struct repeating_timer timer;
volatile bool timer_fired{ false };

void setup() {  // the setup function runs once
  Serial.begin(115200);
  analogReadResolution(12);     // default is 10
  analogWriteFreq(60000);       // 60KHz, about max
  analogWriteRange(DAC_RANGE);  // 100% duty cycle
  r = lux_to_volt(r);
  // Gain measurement at the beginning of the program
  my_desk.setGain(Gain());
  Serial.printf("The static gain of the system is %f\n", my_desk.getGain());
  my_pid.set_b((1/(my_pid.get_k()* my_desk.getGain())));
  Serial.printf("%f %f\n", my_pid.get_k(), my_pid.get_b());
  add_repeating_timer_ms(-10, my_repeating_timer_callback, NULL, &timer);
}

void loop() {  // the loop function runs cyclically
  int j, u;
  float v_adc, total_adc;
  unsigned long time;
  // Média de 20 medições, para reduzir noise
  if (timer_fired) {
    time = micros();
    timer_fired = false;
    for (j = 0, total_adc = 0; j < 20; j += 1) {
      read_adc = analogRead(A0);
      total_adc += read_adc;
    }
    read_adc = total_adc / 20.0;

    if (!my_desk.isIgnoreReference()) {
      // Feedforward
      my_pid.compute_feedforward(r);
      if (my_pid.get_feedback()) {
        v_adc = adc_to_volt(read_adc);         // Volt na entrada
        u = my_pid.compute_control(r, v_adc);  // Volt
        my_pid.housekeep(r, v_adc);
      } else {
        u = my_pid.get_u();
      }
      analogWrite(LED_PIN, u);
      my_desk.setDutyCycle(u / dutyCycle_conv);
    }
    float lux = adc_to_lux(read_adc);
    my_desk.Compute_avg(my_pid.get_h(), lux, reference);
    my_desk.store_buffer(lux);
    read_command();
    real_time_stream_of_data(time / 1000, lux);
  }
}

// Conversões
float adc_to_volt(int read_adc) {  // Passar do valor lido para volts
  return read_adc / adc_conv;
}

int volt_to_adc(float input_volt) {
  return input_volt * adc_conv;
}

float adc_to_lux(int read_adc) {
  float LDR_volt;
  LDR_volt = read_adc / adc_conv;
  return volt_to_lux(LDR_volt);
}

float lux_to_volt(float lux) {
  float resistance = pow(10, (my_desk.getM() * log10(lux) + my_desk.getOffset_R_Lux()));
  return (VCC * 10000.0) / (resistance + 10000.0);
}

float volt_to_lux(float volt) {
  float LDR_resistance = (VCC * 10000.0) / volt - 10000.0;
  return pow(10, (log10(LDR_resistance) - my_desk.getOffset_R_Lux()) / (my_desk.getM()));
}


float Gain()  // Calcula o ganho da caixa em cada run
{
  analogWrite(LED_PIN, 819);  // 0.2 de duty cycle
  delay(2500);
  float y1 = adc_to_volt(analogRead(A0));
  analogWrite(LED_PIN, 4095);
  delay(2500);
  float y2 = adc_to_volt(analogRead(A0));
  analogWrite(LED_PIN, 0);
  delay(2500);
  return (y2 - y1)/(4095 - 819);
}

bool my_repeating_timer_callback(struct repeating_timer *t) {
  if (!timer_fired) {
    timer_fired = true;
  }
  return true;
}