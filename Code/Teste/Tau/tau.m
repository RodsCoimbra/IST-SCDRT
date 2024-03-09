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
plot(Tau)

%% Simular resposta
lumen = 100;
vf = calc_vf(lumen);
vi = 0;
t = 0:0.001:1;
tau_ = calc_tau(lumen);
v = vf - (vf - vi)*exp(-t/(tau_));
plot(t,v)

function vf= calc_vf(x)
    m = -0.88;
    b =  log10(225000)-m;
    R1 = 10 *10^3;
    R2_value = 10^(m*log10(x) + b);
    vf = 3.3 * (R1)/(R1+ R2_value);
end

function Tau= calc_tau(x)
    m = -0.88;
    b =  log10(225000)-m;
    C = 10 * 10^-6;
    R1 = 10 *10^3;
    R2_value = 10^(m*log10(x) + b);
    Req = (R2_value*R1)/(R2_value+R1);
    Tau = Req * C;
end


