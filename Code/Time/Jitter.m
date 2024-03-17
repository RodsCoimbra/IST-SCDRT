Jit = load("Jitter.csv");
Jit = (Jit(5:end) - 10)*10^3;
plot(Jit, LineWidth=1.5)
grid on
xlabel("Sample")
ylabel("Microsecond [\mus]")
xlim([0 3740])