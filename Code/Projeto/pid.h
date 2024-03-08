#ifndef PID_H
#define PID_H
#define Max_u 3.3
#define Min_u 0


class pid {
  float I, D, K, P, Ti, Td, Tt, b, h, y_old, K_old, b_old, N, u_fb, u_ff, u, v;
  bool anti_windup, feedback;
public:
  explicit pid(float _h, float _K = 1, float b_ = 1, float Ti_ = 1, float Tt_ = 1, float Td_ = 0, float N_ = 10);
  ~pid(){};
  int compute_control(float r, float y);
  void compute_feedforward(float r);
  void set_antiwindup(bool set);
  bool get_antiwindup();
  void set_feedback(bool set);
  bool get_feedback();
  void housekeep(float r, float y);
  float get_u();
};

inline void pid::housekeep(float r, float y) {
  float e = r - y;
  const float ao = h / Tt;
  float bi = K * h / Ti;
  I += bi * (e);
  if (anti_windup) {
    I += ao * (u - v);
  }
  y_old = y;
}
#endif  //PID_H