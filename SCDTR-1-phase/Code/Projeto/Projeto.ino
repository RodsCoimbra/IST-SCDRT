#include "pid.h"
#include "command.h"
#include "lumminaire.h"

const int LED_PIN = 15;
const int DAC_RANGE = 4096;
const float VCC = 3.3;
const float adc_conv = 4095.0 / VCC;
const float dutyCycle_conv = 4095.0 / 100.0;
pid my_pid{0.01, 0.15, 1.5}; // h, k, Tt
// (float _h, float _K, float Tt_,float b_,float Ti_, float Td_, float N_)

lumminaire my_desk{-0.89, log10(225000) - (-0.89), 0.0158, 1}; // m, b(offset), Pmax, desk_number
// system my_desk{float _m, float _offset_R_Lux, float _Pmax, unsigned short _desk_number}

float ref{10.0};
float ref_volt;
float read_adc;
bool debbuging = false;
struct repeating_timer timer;
volatile bool timer_fired{false};

void setup()
{ // the setup function runs once
  Serial.begin(115200);
  analogReadResolution(12);    // default is 10
  analogWriteFreq(60000);      // 60KHz, about max
  analogWriteRange(DAC_RANGE); // 100% duty cycle
  ref_volt = lux_to_volt(ref);
  // Gain measurement at the beginning of the program
  Gain();
  add_repeating_timer_ms(-10, my_repeating_timer_callback, NULL, &timer);
}

void loop()
{ // the loop function runs cyclically
  float u;
  int pwm;
  float v_adc, total_adc;
  float time;
  if (timer_fired)
  {
    time = millis();
    timer_fired = false;
    read_adc = digital_filter(20.0);
    if (my_desk.isON() && (!my_desk.isIgnoreReference()))
    {
      // Feedforward
      my_pid.compute_feedforward(ref_volt);

      // Feedback
      if (my_pid.get_feedback())
      {
        v_adc = adc_to_volt(read_adc);               // Volt na entrada
        u = my_pid.compute_control(ref_volt, v_adc); // Volt
        my_pid.housekeep(ref_volt, v_adc);
      }
      else
      {
        u = my_pid.get_u();
      }
      pwm = u * 4095;
      analogWrite(LED_PIN, pwm);
      my_desk.setDutyCycle(pwm / dutyCycle_conv);
    }
    float lux = adc_to_lux(read_adc);
    my_desk.Compute_avg(my_pid.get_h(), lux, ref);
    my_desk.store_buffer(lux);
    read_command();
    real_time_stream_of_data(time / 1000, lux);
  }
}

// Conversões
float adc_to_volt(int read_adc)
{
  return read_adc / adc_conv;
}

int volt_to_adc(float input_volt)
{
  return input_volt * adc_conv;
}

float adc_to_lux(int read_adc)
{
  float LDR_volt;
  LDR_volt = read_adc / adc_conv;
  return volt_to_lux(LDR_volt);
}

float lux_to_volt(float lux)
{
  float resistance = pow(10, (my_desk.getM() * log10(lux) + my_desk.getOffset_R_Lux()));
  return (VCC * 10000.0) / (resistance + 10000.0);
}

float volt_to_lux(float volt)
{
  float LDR_resistance = (VCC * 10000.0) / volt - 10000.0;
  return pow(10, (log10(LDR_resistance) - my_desk.getOffset_R_Lux()) / (my_desk.getM()));
}

// Funções extras
void Gain()
{
  Serial.println("Calibrating the gain of the system:");
  analogWrite(LED_PIN, 0);
  delay(2500);
  float y1 = adc_to_lux(digital_filter(50.0));
  analogWrite(LED_PIN, 4095);
  delay(2500);
  float y2 = adc_to_lux(digital_filter(50.0));
  analogWrite(LED_PIN, 0);
  delay(2500);
  float Gain = (y2 - y1);
  my_desk.setGain(Gain);
  my_pid.set_b(lux_to_volt(ref) / ref, Gain);
  Serial.printf("The static gain of the system is %f [LUX/DC]\n", Gain);
}

bool my_repeating_timer_callback(struct repeating_timer *t)
{
  if (!timer_fired)
  {
    timer_fired = true;
  }
  return true;
}

// Média de medições, para reduzir noise;
float digital_filter(float value)
{
  float total_adc;
  int j;
  for (j = 0, total_adc = 0; j < value; j += 1)
  {
    total_adc += analogRead(A0);
  }
  return total_adc / value;
}

void ref_change(float value)
{
  ref = value;
  ref_volt = lux_to_volt(ref);
  my_desk.setIgnoreReference(false);
  my_pid.set_b(lux_to_volt(ref) / ref, my_desk.getGain());
  my_desk.setON(true);
  my_pid.set_Ti(Tau(ref));
}

float Tau(float value)
{
  if (value >= 0.5)
  {
    float R1 = 10e3;
    float R2 = pow(10, (my_desk.getM() * log10(value) + my_desk.getOffset_R_Lux()));
    float Req = (R2 * R1) / (R2 + R1);
    return Req * 10e-6;
  }
  else
  {
    return 0.1;
  }
}