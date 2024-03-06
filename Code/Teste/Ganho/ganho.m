clear
close all
value = load("Teste.csv");
len = length(value(:,1));
m = -0.9:0.01:-0.7;
G = zeros(len/15, 14);
for j = 1:(len/15)
    figure();
    hold on
    k = j -1;
    for i = 1:(15-1)
        G(j, i) = (value(k * 15 + i+1,2) - value(k * 15 + i,2))/(value(k * 15 + i+1,1) - value(k * 15 + i,1));
    end
        k = (j-1) * 15;
        plot(value((k+1):(k+15),1), value((k+1):(k+15),2))
        title(["Ganho para m =" num2str(m(j))])
        xlabel("Duty-cycle")
        ylabel("Lux")
end


%% Descobrir o erro
close all
G_avg = mean(G, 2);
for i = 1:21
    G(i,:) = G(i,:)/G_avg(i);
end
Erro = zeros(len/15, 1);
for j = 1:(len/15)
    for i = 1:(15-1)
    Erro(j) = Erro(j) + (1 - G(j,i))^2;
    end
end

[er,i] = min(Erro);
sprintf("%f, %f", er, m(i))


%% Teste final

(value(30,2)-value(16,2))/(value(30,1)-value(16,1));