clear
close all
Excel = load("mean.csv");
Excel2 = load("Real_fine_params.csv");

Lux = Excel2(301:3401, 3);

i = (1:3101)';
energy = Excel(1:3101, 1);
energy = energy .* i;

visibility = Excel(1:3101, 2);
visibility = visibility .* i;

flicker = Excel(1:3101, 3);
flicker = flicker .* i; 
x = 0:0.01:31;

%% energy
figure()
yyaxis left;
plot(x,Lux, '--',LineWidth=2)
ylabel("Illuminance [LUX]")
ylim([0 40])
hold on
grid on
yyaxis right;
plot(x, energy, '-', LineWidth=1.5)
xlim([0 30])
ylabel("Energy [Joules]")
xlabel("Time [s]")
legend("Measured illuminance [LUX]", "Accumulated energy consumption [Joules]")

%% vis
figure()
yyaxis left;
plot(x,Lux, '--',LineWidth=2)
ylabel("Illuminance [LUX]")
ylim([0 40])
hold on
grid on
yyaxis right;
plot(x, visibility, '-', LineWidth=1.5)
xlim([0 30])
ylabel("Visibility error [LUX]")
xlabel("Time [s]")
legend("Measured illuminance [LUX]", "Accumulated visibility error [LUX]")
%% flicker

figure()
yyaxis left;
plot(x,Lux, '--',LineWidth=2)
ylabel("Illuminance [LUX]")
ylim([0 40])
hold on
grid on
yyaxis right;
plot(x, flicker, '-', LineWidth=1.5)
xlim([0 30])
ylabel("Flicker error [s^{-1}]")
xlabel("Time [s]")
legend("Measured illuminance [LUX]", "Accumulated flicker error [s^{-1}]")