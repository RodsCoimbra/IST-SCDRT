#include "pid.h"
#include <iostream>

using namespace std;

pid::pid( float _h, float _K, float _Ki, float _Kd, float b_,
float Ti_, float Td_, float N_)
// member variable initialization list
: h {_h}, K{_K}, Ki {_Ki}, Kd {_Kd}, b {b_}, Ti {Ti_}, Td {Td_},
N {N_}, I {0.0}, D {0.0}, y_old{0.0}, saturated{false}
{ } 
float pid::compute_control( float r, float y ) {
float P = K*(b*r-y);
float ad = Td/(Td+N*h);
float bd = Td*Kd*N/(Td+N*h);
D = ad*D-bd*(y-y_old);
u = P+I+D;
if( u < -0.1) {
saturated = true;
}
if( u > 3.4){
saturated = true;}
return u;
}
