clear

h = 0.01;
K = 0.25;
% G = 0.000166; %72.312302
G = 72.312302; 
% G = 0.695055; %Teste
H = 0.014; %Serial.printf("H -> %f\n", lux_to_volt(lux)/lux);
b = 1/(K*G*H);
% b = 3.02;     %Teste
Ti = 0.43;
TT = 0.5;
s = tf('s');
tau = 0.43;
% F = ((b*Ti*s+1)*(s*tau + 1))/((s*tau+1)*(Ti*s/(G*H*K))+ Ti*s + 1);
% step(F)
%controlSystemDesigner(F)