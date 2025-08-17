#include "pidcontroller.h"

//QDEBUG TEMP
#include <QDebug>

PIDController::PIDController(double Kp, double Ki, double Kd, double integralLimit, double initCommand, bool initialized) {
    SetKp(Kp);
    SetKi(Ki);
    SetKd(Kd);
    SetIntegralLimit(integralLimit);
    if(initialized)
        SetInitValue(initCommand);
    else
        _lastCommand = initCommand;
}

double PIDController::Calculate(double error){

    if(!_commandInit)
        return _lastCommand;

    if(!_sumInit){
        _sumInit = true;
        _lastError = error;
        return _lastCommand;
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

    _lastCommand += (_P + _I + _D);
    return _lastCommand;
}

void PIDController::SetInitValue(double value){
    _lastCommand = value;
    _commandInit = true;
}

void PIDController::Reset(){
    _lastError = 0.0;
    _sumError = 0.0;
    _sumInit = false;
    _commandInit = false;
    _P = 0.0;
    _I = 0.0;
    _D = 0.0;
}

void PIDController::ResetSum(){
    _sumError = 0.0;
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

bool PIDController::IsInitialized() const{
    return _commandInit;
}
