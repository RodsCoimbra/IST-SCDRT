clear
close all
%% Tau teórico
C = 10 * 10^-6;
R1 = 10 *10^3;
x = 1:300;
m = -0.88;
b =  log10(225000)-m;
R2 = 10.^(m*log10(x) + b);
Req = (R2*R1)./(R2+R1);
Tau = Req * C;
%plot(Tau)
vf = calc_vf(10);
vi = 0;
vf - (vf - vi)*exp(-1)

function vf= calc_vf(x)
    m = -0.88;
    b =  log10(225000)-m;
    R2_value = 10^(m*log10(x) + b);
    vf = 3.3 *(10 *10^3 /(10 *10^3+ R2_value);
end


% value = load("tau.csv");
% value(:,1) = value(:,1) * 10^-6 - 5;
% plot(value(:,1), value(:,2))