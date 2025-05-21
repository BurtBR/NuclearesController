classdef Maestro
    methods(Static)
        function Run(sampled, order)
            sampled = sampled(:,2);
            sampled = sampled/max(sampled);
            sampled = sampled-sampled(1);
        
            % MQnR_gpt(data, Order)
            Maestro.MQnR_gpt(sampled, order);title("Maestro MQnR");
            % MQR_gpt(data, Order)
            %Maestro.MQR_gpt(sampled, order);title("Maestro MQR");
        
            % MQnR(data, Order)
            %Maestro.MQnR(sampled, order);title("Maestro MQnR");
            % MQR(data, Order)
            %Maestro.MQR(sampled, order);title("Maestro MQR");
        end

        function MQnR(sampled, order)
            % Programa para Identificacion parametrica no recursiva
            figure();
            %u = [0 1 1 1 1 1]
            %y = [0 0.73 1.26 1.55 1.73 1.84]
            u = [0 ones(1, size(sampled,1)-1)];
            y = sampled';
            n = order;
            
            N = length(u); % Numero de datos
            f = N-n; % Orden de filas
            c = 2*n; % Orden de columnas
            
            s = 0;
            d = n;
            t = d;
            for i = 0:n-1 % Calculo de la matriz F(N)
                s = s+1;
                t = d;
                for k = 1:f
                    y1 = -y(t);
                    u1 = u(t);
                    t = t+1;
                    u2(s,k) = u1;
                    y2(s,k) = y1;
                end
                uf = u2';
                yf = y2';
                d = d-1; % Decrementar una posicion para crear una fila
            end
            uf;
            yf;
            F = [yf uf]; % Matriz F(N)
            FT = F';
            
            A = FT*F;
            B = inv(A);
            
            k = n;
            for i = 1:f % Calculo del vector Y(N)
                k = k+1;
                y3 = y(k); %y3(k)
                yn(i) = y3;
            end
            
            YN = yn';
            
            C = FT*YN;
            th = B*C;
            
            for p = 1:n
                num1 = th(p+n);
                num2(p) = num1;
                den1 = th(p);
                den2(p) = den1;
            end
            num = num2;
            den = [1 den2];
            
            j = 0:N-1;
            subplot(2,1,1);
            r = u;
            y4 = filter(num,den,r);
            plot(j,y4)
            y5 = [y' y4'];
            hold on
            plot(j,y,'*')
            grid
            
            %..EQM 
            e = sqrt(sum((y'-y4').^2))/N; %..0.0247

            fprintf("Identificação por MQnR")
            printsys(num,den,'z')
            fprintf("EQM: %f \n", e);
            
        end

        function MQR(sampled, order)
            % Identificacion recursiva modelo de segundo orden - Recursivo
            figure();
            n = order;
            u = [0 ones(1, size(sampled,1)-1)]; % Vector de entradas
            y = sampled'; % Vector de salidas
            t = 0:1:(size(sampled,1)-1);
            r = [t',y'];
            
            %    n = input('Entre el orden del sistema n=');
            %lamda = input('Entre el factor de olvido lamda=');
            %n = 2;
            lamda = 1;
            
               p = 1000*eye(2*n);
              th = [zeros(1,2*n)]';
            phit = [-y(n) -y(n-1) u(n) u(n-1)];
            
            for k=n+1:length(y)
                   l = (p*phit')/(lamda+phit*p*phit');
                   e = y(k)-phit*th;
                  th = th+l*e;
                   p = (1/lamda)*(eye(2*n)-l*phit)*p;
                phit = [-y(k) -y(k-1) u(k) u(k-1)];
            end
            
            u1 = ones(1, size(sampled,1));
            n = [th(3) th(4)];
            d = [1 th(1) th(2)];
            
            y1 = dlsim(n,d,u1);
            plot(t,y1)
            hold on
            plot(t,y,'*')
            grid
            
            
            %..EQM 
            N = length(y);
            e2 = sqrt(sum((y'-y1).^2))/N; %..0.0131
            
            fprintf("Identificação por MQR")
            printsys(n,d,'z')
            fprintf("EQM: %f \n", e2);

        end
    
        function MQnR_gpt(sampled, order)
            figure();
            u = [0 ones(1, size(sampled,1)-1)];
            y = sampled';
            n = order;
            
            N = length(u); % Número total de amostras
            f = N - n;     % Quantidade de linhas para regressão
            PHI = zeros(f, 2*n); 
            YN = zeros(f, 1);
        
            for k = n+1:N
                phi_y = -y(k-1:-1:k-n);
                phi_u = u(k-1:-1:k-n);
                PHI(k-n, :) = [phi_y, phi_u];
                YN(k-n) = y(k);
            end
        
            th = (PHI' * PHI) \ (PHI' * YN);
        
            num = th(n+1:end)';   % Coeficientes do numerador
            den = [1 th(1:n)'];   % Coeficientes do denominador
        
            j = 0:N-1;
            subplot(2,1,1);
            y_est = filter(num, den, u);
            plot(j, y_est)
            hold on
            plot(j, y, '*')
            grid on
        
            % EQM
            e = sqrt(sum((y - y_est').^2)) / N;
        
            fprintf("Identificação por MQnR\n")
            printsys(num, den, 'z')
            fprintf("EQM: %f \n", sum(e));
        end

        function MQR_gpt(sampled, order)
            figure();
            n = order;
            u = [0 ones(1, size(sampled,1)-1)];
            y = sampled';
            t = 0:1:(size(sampled,1)-1);
            
            lamda = 1;
            th = zeros(2*n, 1);
            P = 1000 * eye(2*n);
        
            % Inicializar phit
            k = n + 1;
            phi_y = -y(k-1:-1:k-n);
            phi_u = u(k-1:-1:k-n);
            phit = [phi_y phi_u];
        
            for k = n+1:length(y)
                phi_y = -y(k-1:-1:k-n);
                phi_u = u(k-1:-1:k-n);
                phit = [phi_y phi_u];
                
                l = (P * phit') / (lamda + phit * P * phit');
                e = y(k) - phit * th;
                th = th + l * e;
                P = (1/lamda) * (eye(2*n) - l * phit) * P;
            end
        
            num = th(n+1:end)';
            den = [1 th(1:n)'];
            y_est = dlsim(num, den, ones(1, size(sampled,1)));
        
            plot(t, y_est)
            hold on
            plot(t, y, '*')
            grid on
        
            % EQM
            N = length(y);
            e2 = sqrt(sum((y - y_est').^2)) / N;
        
            fprintf("Identificação por MQR\n")
            printsys(num, den, 'z')
            fprintf("EQM: %f \n", sum(e2));
        end

    end
end