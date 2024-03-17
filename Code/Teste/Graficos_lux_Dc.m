clear
close all
Excel = load("Real_fine_params.csv");
% Lux = Excel(301:3401, 1);
% Dc = Excel(301:3401, 2);
Lux = Excel(301:3401, 3);
Dc = Excel(301:3401, 4);
% teste = [zeros(100, 1); teste(1:3001)];
x = 0:0.01:31;
%% steps
% Define the ranges and corresponding values
ranges = [0, 500, 501, 1100, 1100, 1800, 1800, 2200, 2200, 3101];
values = [0, 10, 25, 5, 25];

% Initialize an array with zeros
reference = zeros(1, ranges(end));

% Set values according to the specified ranges
for i = 1:2:numel(ranges)-1
    reference(ranges(i)+1:ranges(i+1)) = values((i+1)/2);
end

%% LUX
figure()
yyaxis left;
plot(x,reference, '--',LineWidth=2)
hold on
grid on
plot(x, Lux, 'c-', LineWidth=1.5)
% plot(x, Lux2, 'b-', LineWidth=1.5)
xlabel("Time [s]")
ylabel("Illuminance [LUX]")
ylim([0 40])

yyaxis right;
plot(x, Dc, '-', LineWidth=1.5)
% plot(x, Dc2, 'k-', LineWidth=1.5)
ylabel("DutyCycle [%]")
ylim([0 100])
legend("Reference [LUX]", "Measured illuminance [LUX]", "DutyCycle of the LED [%]")


%% LUX
% plot(x,reference)
% hold on
% grid on
% plot(x, Lux)
% xlim([0 30])
% xlabel("Time [s]")
% ylabel("Illuminance [LUX]")
% legend("Reference [LUX]", "Measured illuminance [LUX]")
% 
% %% Duty cycle
% figure()
% yyaxis left;
% plot(x,result)
% ylabel("Illuminance [LUX]")
% xlim([0 30])
% ylim([0 30])
% hold on
% grid on
% yyaxis right;
% plot(x, Dc)
% xlabel("Time [s]")
% ylabel("DutyCycle [%]")
% legend("Reference [LUX]", "DutyCycle of the LED [%]")
