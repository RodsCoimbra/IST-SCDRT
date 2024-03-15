clear
close all
tau = load("Tau_exp_1023_filter.csv");
plot((tau(:,1)-tau(11,1))*1e-6, tau(:,2))
% grid on
value = 0.63 * (mean(tau(3600:4010,2))-mean(tau(1:11,2)));
yline(value + mean(tau(1:11,2)), '--', '{\tau} (63% of the final value)', LineWidth=0.5)

% ylim([0.3, 0.8]) %PAra 2047 4095
xlim([(tau(1,1)-tau(11,1))*1e-6, (tau(4010,1)-tau(11,1))*1e-6])
xlabel("Time [s]")
ylabel("Analog Input [V]")