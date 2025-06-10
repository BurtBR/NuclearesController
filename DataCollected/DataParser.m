clear; close all; clc;
format longG;

%XenonPlot('XenonEffect.csv');
%return;

namostras = 200;

ordem = 2;

%main('Step70.csv', namostras, ordem);
main('Step75.csv', namostras, ordem);
%main('Step80.csv', namostras, ordem);

function main(fileName, nSample, order)
    Legend = {};
    data = importdata(fileName, ';', 1);

    % The input to the reactor is to remove bars from 100%, so the command
    % at 80 means that 20 were removed. The next lines fix this inversion.
    data.data(:,3) = 100-data.data(:,3);
    data.data(:,4) = 100-data.data(:,4);

    PlotData();

    % Sampling option 1
    % SampleAtValue(Data(y,x,i), Value, QtyOfSamples, endIndex)
    %sampled = Sampling.SampleAtValue(data.data(:,[1 2 4]), 25.0, nSample, 2000);

    % Sampling option 2
    % SampleAtStepInput(Data(y,x,i), QtyOfSamples, endIndex)
    sampled = Sampling.SampleAtStepInput(data.data(:,[1 2 4]), nSample, 2000);

    % Sampling option 3
    % SampleAt(Data(y,x,i), StartIdx, QtyOfSamples, endIndex)
    %sampled = Sampling.SampleAt(data.data(:,[1 2 4]), 100, nSample, 2000);

    %stem(sampled(:,1),sampled(:,2), 'filled', 'Color', [0.0 0.0 0.0]);
    %AppendLegend("Sampled");

    legend(Legend);

    Identification.LeastMeanSquares(sampled, order);
    Identification.RecursiveLeastSquares(sampled, order);

    %Maestro.Run(sampled, order);

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
        plot(data.data(:,1), data.data(:,4), "LineWidth", 3, "Color", [0.3 1.0 0.3]);
        plot(data.data(:,1), data.data(:,3), "LineWidth", 3, "Color", [0.3 0.3 1.0],"LineStyle","-");
        ylim([0 100]);
        ylabel("Rod % removed");
        ax.YColor = 'b';
        
        AppendLegend("Rod Command");
        AppendLegend("Rod Position");
        AppendLegend("Core Temperature");
        xlabel("Time [ms]");
        title("Uncontrolled core");
        yyaxis right;

        xlim([0 8e5]);
        fontsize(20,"points")
    end

    function AppendLegend(text)
        Legend{size(Legend,1)+1,1} = text;
    end
end

function XenonPlot(fileName)

    Legend = {};
    data = importdata(fileName, ';', 1);

    PlotData();

    legend(Legend);

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
        plot(data.data(:,1), data.data(:,4), "LineWidth", 3, "Color", [0.3 1.0 0.3]);
        plot(data.data(:,1), data.data(:,3), "LineWidth", 3, "Color", [0.3 0.3 1.0],"LineStyle","-");
        ylim([0 100]);
        ylabel("Concentration %");
        ax.YColor = 'b';
        
        AppendLegend("Xenon Concentration");
        AppendLegend("Iodine Concentration");
        AppendLegend("Core Temperature");
        xlabel("Time [ms]");
        title("Uncontrolled core");
        yyaxis right;

        fontsize(20,"points")
    end

    function AppendLegend(text)
        Legend{size(Legend,1)+1,1} = text;
    end
end