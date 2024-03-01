clear
value = load("tau.csv");
value(:,1) = value(:,1) * 10^-6 - 5
plot(value(:,1), value(:,2))