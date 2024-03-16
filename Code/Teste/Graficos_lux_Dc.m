clear
close all
Lux = load("LUX_steps.csv");
Dc = load("DC_steps17.csv");
teste = load("teste_steps17.csv");
%Lux = load("teste_steps10.csv");
Lux = Lux(1:3101);
% Dc = [zeros(100, 1); Dc(1:3001)];
Dc = Dc(1:3101);
teste = teste(1:3101);
% teste = [zeros(100, 1); teste(1:3001)];
x = 0:0.01:31;
%% steps
% Define the ranges and corresponding values
ranges = [0, 500, 501, 1100, 1100, 1800, 1800, 2200, 2200, 3101];
values = [0, 10, 25, 5, 25];

% Initialize an array with zeros
result = zeros(1, ranges(end));

% Set values according to the specified ranges
for i = 1:2:numel(ranges)-1
    result(ranges(i)+1:ranges(i+1)) = values((i+1)/2);
end

%% LUX
plot(x,result)
hold on
plot(x, teste)
plot(x, Lux, 'c')
xlim([0 30])
xlabel("Time [s]")
ylabel("Illuminance [LUX]")
legend("Reference", "Measured LUX", "LUX original")

%% Duty cycle
figure()
yyaxis left;
plot(x,result)
ylabel("Illuminance [LUX]")
xlim([0 30])
ylim([0 30])
hold on
yyaxis right;
plot(x, Dc)
xlabel("Time [s]")
ylabel("DutyCycle [%]")
legend("Reference [LUX]", "DutyCycle of the LED [%]")