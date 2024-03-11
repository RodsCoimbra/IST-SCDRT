clear
close all
tau = load("tau_exp_4095.csv");
plot((tau(:,1)-tau(1,1))*1e-6, tau(:,2))
value = 0.63 * mean(tau(3500:4000,2));
yline(value, '--')