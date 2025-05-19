classdef Identification

    methods(Static)
        function LeastMeanSquares(data, order)
            switch order
                case 1
                    Identification.LMS_FirstOrder(data)
                case 2
                    Identification.LMS_SecondOrder(data)
                otherwise
                    warning("Least Mean Square with order " + order + " not implemented");
                    return;
            end
        end

        function RecursiveLeastSquares(data, order)

        end
    end
    
    methods(Access=private,Static)

        function coeficients = LMS_FirstOrder(data)
            y = data(:,2);
            N = size(data,1);
            u = data(:,3);
        end

        function coeficients = LMS_SecondOrder(data)

        end

        function result = FirstOrder(a, b)

        end

    end

end