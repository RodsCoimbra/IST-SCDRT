clear

h = 0.01;
K = 2000;
G = 0.000166;
% G = 0.695055; %Teste
b = 1/(K*G);
% b = 3.02;     %Teste
Ti = 0.43;
TT = 0.5;
s = tf('s');
tau = 0.43;
% H = 33.2;
% F = ((b*Ti*s+1)*(s*tau + 1))/((s*tau+1)*(Ti*s/(G*H*K))+ Ti*s + 1);
% step(F)
%controlSystemDesigner(F)