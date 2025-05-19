classdef Sampling
    methods(Static)

        function result = SampleAt(data, idx, N, endIdx)

            if(size(data,2) < 3)
                warning("The data must have 3 columns, the third being the input");
                return;
            end

            if endIdx <= idx
                warning("End index bigger than start index at Sampling");
                return;
            end

            lenData = size(data,1);
            result = zeros(N,3);
            idxResult = 1;

            periodN = round((endIdx-idx)/N);
            
            while(N > 0 && idx < lenData)
                result(idxResult,1) = data(idx,1);
                result(idxResult,2) = data(idx,2);
                result(idxResult,3) = data(idx,3);
                idxResult = idxResult+1;
                N = N-1;
                idx = idx + periodN;
            end

            if(N > 0)
                result = result(1:end-N,:);
            end
        end

        function result = SampleAtValue(data, startValue, N, endIdx)
            idx = 1;
            lenData = size(data,1);
    
            while(data(idx,2) < startValue && idx < lenData)
                idx = idx + 1;
            end
    
            result = Sampling.SampleAt(data, idx, N, endIdx);
        end

        function result = SampleAtStepInput(data, N, endIdx)
            idx = 1;
            lenData = size(data,1);
    
            startValue = data(idx,3);
            idx = idx+1;
    
            while(data(idx,3) == startValue && idx < lenData)
                idx = idx + 1;
            end
    
            result = Sampling.SampleAt(data, idx, N, endIdx);
        end

    end
end