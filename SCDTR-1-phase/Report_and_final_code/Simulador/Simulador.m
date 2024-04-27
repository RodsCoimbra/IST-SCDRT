clear
close all
h = 0.01;
K = 0.8;
G = 32.772667; 
Ti = 0.5;
TT = 1.5;
tau = 0.5;
out = sim("Sim.slx");

%% LUX
yyaxis left;
plot(out.ref.Time,out.ref.Data, '--',LineWidth=2)
hold on
grid on
plot(out.LUX.Time, out.LUX.Data, 'c-', LineWidth=1.5)
xlabel("Time [s]")
ylabel("Illuminance [LUX]")

yyaxis right;
plot(out.dc.Time, out.dc.Data*100, '-', LineWidth=1.5)
ylabel("DutyCycle [%]")
ylim([0 100])
legend("Reference [LUX]", "Measured illuminance [LUX]" , "DutyCycle of the LED [%]")