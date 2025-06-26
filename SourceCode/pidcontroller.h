#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H

#include <QList>

class PIDController{
private:
    double _lastError = 0.0, _sumError = 0.0, _integralLimit = 100.0, _lastCommand = 0.0;
    double _Kp, _Ki, _Kd, _P = 0.0, _I = 0.0, _D = 0.0;
    bool _sumInit = false, _wasNegative = false, _commandInit = false;
public:
    PIDController(double Kp = 1.0, double Ki = 0.0, double Kd = 0.0, double integralLimit = 0.1, double initCommand = 0.0, bool initialized = false);

    double Calculate(double error);
    void Reset();
    void ResetSum();

    void SetInitValue(double value);
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
    bool IsInitialized() const;
};

#endif // PIDCONTROLLER_H
