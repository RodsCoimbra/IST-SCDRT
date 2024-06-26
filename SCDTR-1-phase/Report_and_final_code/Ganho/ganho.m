clear
close all
value = load("Ganho_final.csv");
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
close all
figure();
color = get(gca,'ColorOrder');
% color = [color(5,:)];
i = 0;
hold on
legend_entries = cell(1, 6);
for j = 1:10:(len/15)
    i = i+1;
    k = (j-1) * 15;
    plot(value((k+1):(k+15),1), value((k+1):(k+15),2), 'o', 'Color',color(i, :),'MarkerSize',5)
    legend_entries{i*2-1} = sprintf('m= %f', m(j));
    xlabel("Duty-cycle [%]")
    ylabel("Measured illuminance [Lux]")
    legend_entries{i*2} = sprintf(''); % Create legend string
    b1 = value((k+1):(k+15),1)\ value((k+1):(k+15),2);
    plot(value((k+1):(k+15),1), b1*value((k+1):(k+15),1), '--' ,'Color',color(i,:), LineWidth=1);
end
legend(legend_entries)
    % i = i+1;
    % k = 1 * 15;
    % plot(value((k+1):(k+15),1), value((k+1):(k+15),2), 'kx','MarkerSize',9)
    % %title(["Ganho para m =" num2str(m(j))])
    % legend_entries{i*2-1} = sprintf('m= %f', m(2));
    % xlabel("Duty-cycle [%]")
    % ylabel("Measured illuminance [Lux]")
    % legend_entries{i*2} = sprintf(''); % Create legend string
    % b1 = value((k+1):(k+15),1)\ value((k+1):(k+15),2);
    % plot(value((k+1):(k+15),1), b1*value((k+1):(k+15),1), '-' ,'Color',color(1,:), 'Linewidth', 1.5);
    % legend(["Data when m = -0.89" "Linear regression for m = -0.89"]);