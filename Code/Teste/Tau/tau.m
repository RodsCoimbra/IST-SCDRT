clear
close all
%% Tau teórico
C = 10 * 10^-6;
R1 = 10 *10^3;
x = 1:300;
m = -0.8;
b =  log10(225000)-m;
R2 = 10.^(m*log10(x) + b);
Req = (R2*R1)./(R2+R1);
Tau = Req * C;
plot(Tau)
% value = load("tau.csv");
% value(:,1) = value(:,1) * 10^-6 - 5;
% plot(value(:,1), value(:,2))