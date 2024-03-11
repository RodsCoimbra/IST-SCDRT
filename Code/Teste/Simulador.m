clear

h = 0.005;
K = 55555;
b = 0.1;
G = 0.000180;
Ti = 0.1;
TT = 0.1;
s = tf('s');
tau = 0.1;
F = ((b*Ti*s+1)*(s*tau + 1))/((s*tau+1)*(Ti*s/(G*h*K))+ Ti*s + 1);
controlSystemDesigner(F)