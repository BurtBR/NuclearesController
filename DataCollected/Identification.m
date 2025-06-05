classdef Identification

    methods(Static)
        function LeastMeanSquares(data, order)
            switch order
                case 1
                    Identification.LMS_FirstOrder(data(:,3), data(:,2));
                case 2
                    Identification.LMS_SecondOrder(data(:,3), data(:,2));
                otherwise
                    warning("Least Mean Square with order " + order + " not implemented");
                    return;
            end
        end

        function RecursiveLeastSquares(data, order)
            switch order
                case 1
                    RLS_FirstOrder(data(:,3), data(:,2));
                case 2
                    RLS_SecondOrder(data(:,3), data(:,2));
                otherwise
                    warning("Recursive Least Squares with order " + order + " not implemented");
                    return;
            end
        end
    end
    
    methods(Access=private,Static)

        function coeficients = LMS_FirstOrder(input, output)

            % https://www.youtube.com/watch?v=H_qrxFZjM2c
            
            minoutput = min(output);
            
            output = output-minoutput;
            y = flip(output);
            u = flip(input);
            phi = [-[y(2:end)] [u(2:end)]];
            y = y(1:end-1);
            coeficients = pinv(phi)*y;

            num = coeficients(2);
            den = [1 coeficients(1)];
            printsys(num, den,'z');

            t = 1:size(output,1);
            figure();
            estimation = dlsim(num, den, input) + minoutput;
            scatter(t, output+minoutput, 'o', 'MarkerEdgeColor', [1.0 0.5 0.5],'LineWidth', 1);hold on;
            plot(t, estimation, 'LineWidth', 3,'Color',[0.5 0.5 1.0]);
            legend("Experiment", "Estimation");
            xlabel("n");
        end

        function coeficients = LMS_SecondOrder(input, output)

            % https://www.youtube.com/watch?v=H_qrxFZjM2c

            minoutput = min(output);

            output = output-minoutput;
            y = flip(output);
            u = flip(input);
            phi = [-[y(2:end-1)] -[y(3:end)] [u(2:end-1)] [u(3:end)]];
            y = y(1:end-2);
            coeficients = pinv(phi)*y;

            num = coeficients(3:4)';
            den = [1 coeficients(1:2)'];
            printsys(num, den,'z');

            t = 1:size(output,1);
            figure();
            estimation = dlsim(num, den, input) + minoutput;
            scatter(t, output+minoutput, 'o', 'MarkerEdgeColor', [1.0 0.5 0.5],'LineWidth', 1);hold on;
            plot(t, estimation, 'LineWidth', 3,'Color',[0.5 0.5 1.0]);
            legend("Experiment", "Estimation");
            xlabel("n");
        end

        function coeficients = RLS_FirstOrder(input, output)

        end

        function coeficients = RLS_SecondOrder(input, output)

        end

    end

end