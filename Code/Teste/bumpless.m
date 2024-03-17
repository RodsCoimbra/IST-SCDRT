clear
close all
bump = load("Bumpless.csv");
x = 0:0.01:27.98;
time = 6;

% Define the ranges and corresponding values
ranges = [0, 150, 150, 400, 400, 650, 650, 1200, 1200, 2000];
values = [0.25, 5, 0.25, 5, 0.25];


% Initialize an array with zeros
result = zeros(1, ranges(end));

% Set values according to the specified ranges
for i = 1:2:numel(ranges)-1
    result(ranges(i)+1:ranges(i+1)) = values((i+1)/2);
end

f = figure;
f.Position = [500 200 800 500];
yyaxis left;
plot(x-time, bump, LineWidth=1)
hold on
grid on
yline(10, 'r--', LineWidth=1.5)
ylim([0 20])
ylabel("Illuminance [LUX]")
yyaxis right;
plot(x(1:2000),result, LineWidth=1)
xline(6.0, '--', 'Bumpless Transfer Off', LabelHorizontalAlignment = 'right', LabelOrientation = 'horizontal', LineWidth=1)
xline(6.0, '--', 'Bumpless Transfer On', LabelHorizontalAlignment = 'left', LabelOrientation = 'horizontal', LineWidth=1)
% xline(7.5-time)
% xline(10-time)
% xline(12.5-time)
% xline(20-time)
xlim([0 20])
ylim([0 6])
xlabel("Time [s]")
ylabel("Value of K")
legend("Measured illuminance [LUX]","Reference [LUX]","Parameter K", "Bumpless Mode Switch", "")
