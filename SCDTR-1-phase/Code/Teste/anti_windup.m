clear
close all
windup = load("windup.csv");
dc = load("windup_dc.csv");
x = 0:0.01:(numel(windup)-1)*0.01;
time = 0;

% Define the ranges and corresponding values
ranges = [0, 1000, 1000, 2000, 2000, 3000, 3000, numel(windup)];
values = [5, 20, 5, 20];


% Initialize an array with zeros
result = zeros(1, ranges(end));

% Set values according to the specified ranges
for i = 1:2:numel(ranges)-1
    result(ranges(i)+1:ranges(i+1)) = values((i+1)/2);
end

f = figure;
f.Position = [500 200 800 500];


yyaxis left
plot(x-time, windup, LineWidth=1.5)
xlim([0 50])
hold on
grid on
ylabel("Illuminance [LUX]")
plot(x-time,result, 'Color', [0.45 0.85 0.8], LineWidth=2.5)
xline(20, '--', 'Anti-Windup Off', LabelHorizontalAlignment = 'right', LabelOrientation = 'horizontal', LineWidth=1)
xline(20, '--', 'Anti-Windup On', LabelHorizontalAlignment = 'left', LabelOrientation = 'horizontal', LineWidth=1)
ylim([0 30])
yyaxis right
plot(x(1:3:end)-time, dc(1:3:end), LineWidth=1.5)
ylim([0 40])
ylabel("DutyCycle [%]")
xlabel("Time [s]")
legend("Measured illuminance [LUX]","Reference [LUX]","Anti-Windup Mode Switch", "", "DutyCycle [%]")