#include "workerdatacollections.h"

WorkerDataCollections::WorkerDataCollections(QObject *parent) : QObject{parent}{

}

WorkerDataCollections::~WorkerDataCollections(){
    if(_timer){
        delete _timer;
        _timer = nullptr;
    }
}

void WorkerDataCollections::StartCollection(int msec, QString filename){

    if(_timer){
        emit Message("Collection already started!", MainWindow::ConsoleMessageType::Error);
        emit Error();
        return;
    }

    try{
        _timer = new QTimer(this);
    }catch(...){
        emit Message("Failed to allocate memory for data collection", MainWindow::ConsoleMessageType::Error);
        emit Error();
        return;
    }

    connect(_timer, &QTimer::timeout, this, &WorkerDataCollections::Timeout);
    _timer->start(msec);
}

void WorkerDataCollections::StopCollection(){

}

void WorkerDataCollections::Timeout(){

}
