clear; close all; clc;
format longG;

period = 23.4;
Z = tf([1.2237 -0.12052],[1 -1.2012 0.28657], period);
PlotFinalAnalisis(Z);

return;

namostras = 25;
ordem = 2;

%PlotTestData('Step70.csv', 5:10:1000, ordem);
%PlotTestData('Step75.csv', 5:10:1000, ordem);
%PlotTestData('Step80.csv', 5:10:1000, ordem);

%XenonPlot('XenonEffect.csv');

%main('Step70.csv', namostras, ordem);
main('Step75.csv', namostras, ordem);
%main('Step80.csv', namostras, ordem);

function PlotFinalAnalisis(Z)
    Z
    S = d2c(Z)

    figure();
    rlocus(Z);
    pbaspect([1 1 1]);
    daspect([1 1 1]);
    fontsize(20,"points");
    set(findall(gca, 'Type', 'Line'),'LineWidth',5);
    set(findall(gca, 'Type', 'Line'),'MarkerSize',20);

    % figure();
    % pzmap(Z);
    % fontsize(20,"points");
    % pbaspect([1 1 1]);
    % daspect([1 1 1]);
    % set(findall(gca, 'Type', 'Line'),'LineWidth',5);
    % set(findall(gca, 'Type', 'Line'),'MarkerSize',20);

    figure();
    margin(Z);
    fontsize(20,"points");
    set(findall(gca, 'Type', 'Line'),'LineWidth',2);
end

function rmseError = testDataLMS(fileName, nSample, order)
    data = importdata(fileName, ';', 1);

    % The input to the reactor is to remove bars from 100%, so the command
    % at 80 means that 20 were removed. The next lines fix this inversion.
    data.data(:,3) = 100-data.data(:,3);
    data.data(:,4) = 100-data.data(:,4);

    sampled = Sampling.SampleAtStepInput(data.data(:,[1 2 4]), nSample, 2000);
    
    coefficients = Identification.LeastMeanSquares(sampled, order);

    input = sampled(:,3);
    output = sampled(:,2);

    startValue = min(output);
    t = 0:(size(output,1)-1);
    estimation = dlsim(coefficients(order+1:end)', [1 coefficients(1:order)'], input)+startValue;

    rmseError = rmse(output, estimation);
end

function rmseError = testDataRMS(fileName, nSample, order)
    data = importdata(fileName, ';', 1);

    % The input to the reactor is to remove bars from 100%, so the command
    % at 80 means that 20 were removed. The next lines fix this inversion.
    data.data(:,3) = 100-data.data(:,3);
    data.data(:,4) = 100-data.data(:,4);

    sampled = Sampling.SampleAtStepInput(data.data(:,[1 2 4]), nSample, 2000);
    
    coefficients = Identification.RecursiveLeastSquares(sampled, order);

    input = sampled(:,3);
    output = sampled(:,2);

    startValue = min(output);
    t = 0:(size(output,1)-1);
    estimation = dlsim(coefficients(order+1:end)', [1 coefficients(1:order)'], input)+startValue;

    rmseError = rmse(output, estimation);
end

function PlotTestData(fileName, nSample, order)
        figure(Name=fileName);hold on;

        errors = [];

        for k = nSample
            errors = [errors;
                      k testDataLMS(fileName, k, order)];
        end
        
        plot(errors(:,1), errors(:,2), LineWidth=5, Color=[1.0 0.5 0.5]);

        errors = [];

        for k = nSample
            errors = [errors;
                      k testDataRMS(fileName, k, order)];
        end
        
        plot(errors(:,1), errors(:,2), LineWidth=5, Color=[0.5 0.5 1.0]);

        title("Identification Test - Order " + order);
        xlabel("Samples");
        ylabel("Root mean squared error");
        ylim([0 10]);
        fontsize(20,"points");
        legend("LMS", "RLS");
    end

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

    stem(sampled(:,1),sampled(:,2), 'filled', 'Color', [0.0 0.0 0.0]);
    AppendLegend("Sampled");

    legend(Legend);

    minOutput = min(sampled(:,2));
    input = sampled(:,3);
    output = sampled(:,2)-minOutput;

    coefficients = Identification.LeastMeanSquares(sampled, order);
    Identification.PlotSystemEstimation(coefficients(order+1:end)', [1 coefficients(1:order)'], input, output, minOutput);
    title("Least Mean Squares");

    coefficients = Identification.RecursiveLeastSquares(sampled, order);
    Identification.PlotSystemEstimation(coefficients(order+1:end)', [1 coefficients(1:order)'], input, output, minOutput);
    title("Recursive Least Squares");

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
        fontsize(20,"points");
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