clear
close all
tau = load("Tau_exp_2047_4095.csv");
plot((tau(:,1)-tau(101,1))*1e-6, tau(:,2))
grid on
value = 0.63 * (mean(tau(3600:4101,2))-mean(tau(1:100,2)));
yline(value + mean(tau(1:100,2)), '--', LineWidth=0.5)

% ylim([0.3, 0.8]) %PAra 2047 4095
xlim([(tau(1,1)-tau(101,1))*1e-6, (tau(4101,1)-tau(101,1))*1e-6])