#include "pid.h"
#include "command.h"
#define buffer_size 6000
const int LED_PIN = 15;
const int DAC_RANGE = 4096;
const float VCC = 3.3;
const float adc_conv = 4095.0 / VCC;
const float dutyCycle_conv = 4095.0 / 100.0;
const float m = -0.89;
const float offset_R_Lux = log10(225000) - m;
const float Pmax = 0.0158;
pid my_pid{0.01, 5000, 0.75, 0.434, 0.1};
//(h+, K, b ,Ti, TT
// pid my_pid {5, 8, 3, 0, 0.3, 5};
float r{0.0};
float reference = r;
float DutyCycle = 0, G, read_adc;
bool occupied = false, lux_flag = false, duty_flag = false, ignore_reference = false, buffer_full = false;
bool debbuging = true;
short desk = 1;
float last_minute_buffer_l[buffer_size], last_minute_buffer_d[buffer_size];
struct repeating_timer timer;
volatile bool timer_fired{false};

void setup()
{ // the setup function runs once
  Serial.begin(115200);
  analogReadResolution(12);    // default is 10
  analogWriteFreq(60000);      // 60KHz, about max
  analogWriteRange(DAC_RANGE); // 100% duty cycle
  r = lux_to_volt(r);
  // Gain measurement at the beginning of the program
  G = Gain();
  Serial.printf("The static gain of the system is %f\n", G);
  // my_pid.set_b((1/(my_pid.get_k()*G)));
  Serial.printf("%f %f\n", my_pid.get_k(), my_pid.get_b());
  add_repeating_timer_ms(-10, my_repeating_timer_callback, NULL, &timer);
}

void loop()
{ // the loop function runs cyclically
  int j, u;
  float v_adc, total_adc;
  unsigned long time;
  // Média de 50 medições, para reduzir noise
  if (timer_fired)
  {
    time = micros();
    timer_fired = false;
    for (j = 0, total_adc = 0; j < 20; j += 1)
    {
      read_adc = analogRead(A0);
      total_adc += read_adc;
    }
    read_adc = total_adc / 20.0;

    if (!ignore_reference)
    {
      // Feedforward
      my_pid.compute_feedforward(r);
      if (my_pid.get_feedback())
      {
        v_adc = adc_to_volt(read_adc);        // Volt na entrada
        u = my_pid.compute_control(r, v_adc); // Volt
        my_pid.housekeep(r, v_adc);
      }
      else
      {
        u = my_pid.get_u();
      }
      analogWrite(LED_PIN, u);
      DutyCycle = u / dutyCycle_conv;
    }
    store_buffer(time/ 1000000);
    read_command();
    real_time_stream_of_data(time / 1000);
  }
}

// Conversões
float adc_to_volt(int read_adc)
{ // Passar do valor lido para volts
  return read_adc / adc_conv;
}

int volt_to_adc(float input_volt)
{
  return input_volt * adc_conv;
}

float adc_to_lux(int read_adc)
{
  float LDR_volt;
  float LDR_resistance;
  LDR_volt = read_adc / adc_conv;
  LDR_resistance = (VCC * 10000.0) / LDR_volt - 10000.0;
  return pow(10, (log10(LDR_resistance) - offset_R_Lux) / (m));
}

float lux_to_volt(float lux)
{
  float resistance = pow(10, (m * log10(lux) + offset_R_Lux));
  return (VCC * 10000.0) / (resistance + 10000.0);
}

float Gain() // Calcula o ganho da caixa em cada run
{
  analogWrite(LED_PIN, 819); // 0.2 de duty cycle
  delay(2500);
  float y1 = adc_to_volt(analogRead(A0));
  analogWrite(LED_PIN, 4095);
  delay(2500);
  float y2 = adc_to_volt(analogRead(A0));
  analogWrite(LED_PIN, 0);
  delay(2500);
  return (y2 - y1) / (4095 - 819);
}

bool my_repeating_timer_callback(struct repeating_timer *t)
{
  if (!timer_fired)
  {
    timer_fired = true;
  }
  return true;
}

void store_buffer(int time)
{
  static int i = 0;
  last_minute_buffer_d[i] = time;
  i++;
  if (i == buffer_size)
  {
    i = 0;
    buffer_full = true;
    for (int j = 0; j < buffer_size; j++)
    {
      Serial.printf("%f,", last_minute_buffer_d[j]);
    }
    Serial.println();
  }
}
