#include "pidcontroller.h"

//QDEBUG TEMP
#include <QDebug>

PIDController::PIDController(double Kp, double Ki, double Kd, double integralLimit) {
    SetKp(Kp);
    SetKi(Ki);
    SetKd(Kd);
    SetIntegralLimit(integralLimit);
}

double PIDController::Calculate(double error){

    if(!_init){
        _init = true;
        _lastError = error;
        return 0.0;
    }

    double de = error - _lastError;

    if(error > 0 && _wasNegative){
        _wasNegative = false;
        _sumError = 0;
    }else if(error < 0 && !_wasNegative){
        _wasNegative = true;
        _sumError = 0;
    }

    _lastError = error;
    _sumError += error;

    _P = error*_Kp;
    _I = _sumError*_Ki;
    _D = _Kd*de;

    if(abs(_I) > _integralLimit){
        if(_I>0){
            _sumError = _integralLimit/_Ki;
            _I = _integralLimit;
        }else{
            _sumError = (-1)*_integralLimit/_Ki;
            _I = -(_integralLimit);
        }
    }

    return (_P + _I + _D);
}

void PIDController::Reset(){
    _lastError = 0;
    _sumError = 0;
    _init = false;
    _P = 0;
    _I = 0;
    _D = 0;
}

void PIDController::ResetSum(){
    _sumError = 0;
}

void PIDController::SetIntegralLimit(double value){
    _integralLimit = value;
}

void PIDController::SetKp(double gain){
    _Kp = gain;
}

void PIDController::SetKi(double gain){
    _Ki = gain;
}

void PIDController::SetKd(double gain){
    _Kd = gain;
}

double PIDController::GetIntegralLimit(){
    return _integralLimit;
}

double PIDController::GetKp(){
    return _Kp;
}

double PIDController::GetKi(){
    return _Ki;
}

double PIDController::GetKd(){
    return _Kd;
}

double PIDController::GetP(){
    return _P;
}

double PIDController::GetI(){
    return _I;
}

double PIDController::GetD(){
    return _D;
}
