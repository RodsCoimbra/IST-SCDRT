#include "pid.h"

using namespace std;

pid::pid(float _h, float _K, float b_,
         float Ti_, float Tt_, float Td_, float N_)
    : h{_h}, K{_K}, b{b_}, Ti{Ti_}, Td{Td_}, Tt{Tt_},
      N{N_}, I{0.0}, D{0.0}, y_old{0.0}, b_old{b_}, K_old{_K}, anti_windup{true}, feedback{true}
{
}

int pid::compute_control(float r, float y)
{
  I = I + K_old * (b_old * r - y) - K * (b * r - y); // Bumpless
  b_old = b;
  K_old = K;
  P = K * (-y);
  float ad = Td / (Td + N * h);
  float bd = Td * K * N / (Td + N * h);
  D = ad * D - bd * (y - y_old);
  u_fb = P + I + D;
  v = u_ff + u_fb;
  u = v;
  if (anti_windup)
  {
    if (u > 4095)
    {
      u = 4095.0;
    }
    else if (u < 0)
    {
      u = 0.0;
    }
  }
  return u;
}

int pid::get_u()
{
  v = u_ff + u_fb;
  u = v;
  if (anti_windup)
  {
    if (u > 4095)
    {
      u = 4095.0;
    }
    else if (u < 0)
    {
      u = 0.0;
    }
  }
  return u;
}
void pid::compute_feedforward(float r)
{
  u_fb = 0;
  u_ff = (r * b * K);
}

void pid::set_antiwindup(bool set)
{
  anti_windup = set;
  return;
}

bool pid::get_antiwindup()
{
  return anti_windup;
}

void pid::set_feedback(bool set)
{
  if (feedback == true && set == false)
  { // Quando se desativa o feedback, meter o integral a 0
    I = 0.0;
  }
  feedback = set;
  return;
}

bool pid::get_feedback()
{
  return feedback;
}

void pid::set_b(float b_)
{
  b = b_;
}

void pid::set_k(float k_)
{
  K = k_;
}

float pid::get_k()
{
  return K;
}
float pid::get_b()
{
  return b;
}

float pid::get_h()
{
  return h;
}

float pid::get_Ti()
{
  return Ti;
}

float pid::get_Tt()
{
  return Tt;
}

float pid::get_Td()
{
  return Td;
}

float pid::get_N()
{
  return N;
}