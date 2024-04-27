clear
close all
Excel = load("Disturbance.csv");
Lux = Excel(:, 1);
% Dc = Excel(:, 2);
% x = 0:0.01:31;
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
% yyaxis left;
% plot(x,reference, '--',LineWidth=2)
plot(Lux, 'c-', LineWidth=1.5)
hold on
grid on
xlabel("Time [s]")
ylabel("Illuminance [LUX]")
ylim([0 40])

% yyaxis right;
% plot(x, Dc, '-', LineWidth=1.5)
% ylabel("DutyCycle [%]")
% ylim([0 100])
legend("Measured illuminance [LUX]", "DutyCycle of the LED [%]")