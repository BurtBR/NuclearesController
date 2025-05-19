clear; close all; clc;
format longG;

namostras = 10;

ordem = 1; % MQnR so funciona com grau 1 e MQR com grau 2
% A funcao de MQR foi alterada (linha 97 do arquivo Maestro.m)
% para aumentar em 1 a ordem passada como argumento, apenas para 
% podermos rodar os dois algoritmos de uma unica vez

main('Step70.csv', namostras, ordem);
main('Step75.csv', namostras, ordem);
main('Step80.csv', namostras, ordem);

function main(fileName, nSample, order)
    Legend = {};
    data = importdata(fileName, ';', 1);
    PlotData();

    % Sampling option 1
    % Sample(StartTemperature, QtyOfSamples, endSampleIndex)
    sampled = Sample(25.0, nSample, 2000);

    % Sampling option 2
    % SampleAtStepInput(QtyOfSamples, endSampleIndex)
    % sampled = SampleAtStepInput(nSample, 2000);

    stem(sampled(:,1),sampled(:,2), 'filled', 'Color', [0.0 0.0 0.0]);
    AppendLegend("Sampled");

    legend(Legend);

    sampled = sampled(:,2);
    sampled = sampled/max(sampled);
    sampled = sampled-sampled(1);

    % MQnR(data, Order)
    Maestro.MQnR_gpt(sampled, order);title("Maestro MQnR");
    % MQR(data, Order)
    Maestro.MQR_gpt(sampled, order);title("Maestro MQR");

    function PlotData()
        figure();
        hold on;
        
        yyaxis right;
        plot(data.data(:,1), data.data(:,2), "LineWidth", 3, "Color", [1.0 0.3 0.3]);
        ylim([0 450]);
        ax = gca;
        ax.YColor = 'r';
        ylabel("Temperature °C");
        yyaxis left;
        plot(data.data(:,1), 100-data.data(:,4), "LineWidth", 3, "Color", [0.3 1.0 0.3]);
        plot(data.data(:,1), 100-data.data(:,3), "LineWidth", 3, "Color", [0.3 0.3 1.0],"LineStyle","-");
        ylim([0 100]);
        ylabel("Rod % removed");
        ax.YColor = 'b';
        
        AppendLegend("Rod Command");
        AppendLegend("Rod Position");
        AppendLegend("Core Temperature");
        xlabel("Time [ms]");
        title("Uncontrolled core");
        yyaxis right;
    end

    function result = Sample(startTemp, N, endSample)
        idx = 1;
        idxResult = 1;
        lenData = size(data.data,1);
        result = zeros(N,2);

        while(data.data(idx,2) < startTemp && idx < lenData)
            idx = idx + 1;
        end

        periodN = round((endSample-idx)/N);
        
        while(N > 0 && idx < lenData)
            result(idxResult,1) = data.data(idx,1);
            result(idxResult,2) = data.data(idx,2);
            idxResult = idxResult+1;
            N = N-1;
            idx = idx + periodN;
        end
    end

    function result = SampleAtStepInput(N, endSample)
        idx = 1;
        idxResult = 1;
        lenData = size(data.data,1);
        result = zeros(N,2);

        startValue = data.data(idx,4);
        idx = idx+1;

        while(data.data(idx,4) == startValue && idx < lenData)
            idx = idx + 1;
        end

        periodN = round((endSample-idx)/N);
        
        while(N > 0 && idx < lenData)
            result(idxResult,1) = data.data(idx,1);
            result(idxResult,2) = data.data(idx,2);
            idxResult = idxResult+1;
            N = N-1;
            idx = idx + periodN;
        end
    end

    function AppendLegend(text)
        Legend{size(Legend,1)+1,1} = text;
    end
end