#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H

#include <QList>

class PIDController{
private:
    double _lastError = 0, _sumError = 0, _integralLimit = 100;
    double _Kp, _Ki, _Kd, _P = 0, _I = 0, _D = 0;
    bool _init = false, _wasNegative = false;
public:
    PIDController(double Kp = 1, double Ki = 0, double Kd = 0, double integralLimit = 0.1);

    double Calculate(double error);
    void Reset();
    void ResetSum();

    void SetIntegralLimit(double value);
    void SetKp(double gain);
    void SetKi(double gain);
    void SetKd(double gain);
    double GetIntegralLimit();
    double GetKp();
    double GetKi();
    double GetKd();
    double GetP();
    double GetI();
    double GetD();
};

#endif // PIDCONTROLLER_H
