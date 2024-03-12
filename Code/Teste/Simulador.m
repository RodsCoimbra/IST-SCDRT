clear

h = 0.01;
K = 2000;
G = 0.000166;
% b = 1/(2000*G);
b = 3.02;
Ti = 0.43;
TT = 0.5;
s = tf('s');
tau = 0.43;
% F = ((b*Ti*s+1)*(s*tau + 1))/((s*tau+1)*(Ti*s/(G*h*K))+ Ti*s + 1);
% controlSystemDesigner(F)