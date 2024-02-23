#include "pid.h"
#include <iostream>

using namespace std;

pid::pid( float _h, float _K, float b_,
float Ti_, float Td_, float N_)
// member variable initialization list
: h {_h}, K {_K}, b {b_}, Ti {Ti_}, Td {Td_},
N {N_}, I {0.0}, D {0.0}, y_old{0.0}
{ } 
float pid::compute_control( float r, float y ) {
float P = K*(b*r-y);
float ad = Td/(Td+N*h);
float bd = Td*K*N/(Td+N*h);
D = ad*D-bd*(y-y_old);
float u = P+I+D;
if( u < 0 ) u = 0;
if( u > 3.3 ) u = 3.3;
cout << "P: " << P << " I: " << I << " D: " << D << " u: " << u << endl;
return u;
}
