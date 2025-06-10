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
                    Identification.RLS(order, data(:,3), data(:,2));
                case 2
                    Identification.RLS(order, data(:,3), data(:,2));
                otherwise
                    warning("Recursive Least Squares with order " + order + " not implemented");
                    return;
            end
        end
    end
    
    methods(Access=private,Static)

        %                                b
        % coefficients = [a b c] = ---------------
        %                              z + a
        function coefficients = LMS_FirstOrder(input, output)

            % https://www.youtube.com/watch?v=H_qrxFZjM2c
            
            minoutput = min(output);
            
            output = output-minoutput;
            y = flip(output);
            u = flip(input);
            phi = [-[y(2:end)] [u(2:end)]];
            y = y(1:end-1);
            coefficients = pinv(phi)*y;

            Identification.PlotSystemEstimation([coefficients(2)], [1 coefficients(1)], input, output, minoutput);
        end

        %                               c*z + d
        % coefficients = [a b c d] = ---------------
        %                            z^2 + a*z + b
        function coefficients = LMS_SecondOrder(input, output)

            % https://www.youtube.com/watch?v=H_qrxFZjM2c

            minoutput = min(output);

            output = output-minoutput;
            y = flip(output);
            u = flip(input);
            phi = [-[y(2:end-1)] -[y(3:end)] [u(2:end-1)] [u(3:end)]];
            y = y(1:end-2);
            coefficients = pinv(phi)*y;

            Identification.PlotSystemEstimation(coefficients(3:4)', [1 coefficients(1:2)'], input, output, minoutput);
        end

        function coefficients = RLS(order, input, output)

            % https://en.wikipedia.org/wiki/Recursive_least_squares_filter

            % Start at 0
            minoutput = min(output);
            output = output-minoutput;

            forget_factor = 1.0;
            init_value = 1000.0;

            x = [zeros(order,1) ;input];
            y = [zeros(order,1) ;output];

            phi = [-y(order:-1:1)' x(order:-1:1)'];
            coefficients = zeros(2*order,1);

            P = init_value*eye(2*order);

            for k = (order+1):length(input)
                g = (P*phi')/(forget_factor + phi*P*phi');
                e = y(k)-phi*coefficients;
                coefficients = coefficients + g*e;
                P = (1/forget_factor)*(P-g*phi*P);
                phi = [-y(k:-1:k-order+1)' x(k:-1:k-order+1)'];
            end

            Identification.PlotSystemEstimation(coefficients(order+1:end)', [1 coefficients(1:order)'], input, output, minoutput);

        end

        function PlotSystemEstimation(num, den, input, output, startValue)
            printsys(num, den,'z');

            t = 0:(size(output,1)-1);
            figure();
            estimation = dlsim(num, den, input)+startValue;
            scatter(t, output+startValue, 'o', 'MarkerEdgeColor', [1.0 0.5 0.5],'LineWidth', 1);hold on;
            plot(t, estimation, 'LineWidth', 3,'Color',[0.5 0.5 1.0]);
            legend("Experiment", "Estimation");
            xlabel("n");
        end

    end

end