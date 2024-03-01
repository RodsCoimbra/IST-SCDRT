#ifndef PID_H
#define PID_H
class pid {
float I, D, K, Ti, Td, b, h, y_old, N;
public:
explicit pid( float _h, float _K = 1, float b_ = 1, float Ti_ = 1, float Td_ = 0, float N_ = 10);
~pid() {};
float compute_control( float r, float y);
void housekeep( float r, float y, float v);
};
inline void pid::housekeep( float r, float y, float v) {
float e = r - y;
float bi =K*h/Ti;
float u = v;
bool flag;
flag = false;
if (u > 0.9){u = 3.3; flag = true;}
else if(u < 0){u = 0; flag = true;}
if(flag){
I = I +bi*(r-y)+0.5*(u-v);
}
else{
  I += K*h/Ti*e;
}
y_old = y;
}
#endif //PID_H