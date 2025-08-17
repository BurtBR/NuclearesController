#include "workerdatacollections.h"

#include <QJsonDocument>
#include <QJsonObject>

WorkerDataCollections::WorkerDataCollections(QObject *parent) : QObject{parent}{

}

WorkerDataCollections::~WorkerDataCollections(){
    if(_timer){
        delete _timer;
        _timer = nullptr;
    }
    if(_elapsedTimer){
        delete _elapsedTimer;
        _elapsedTimer = nullptr;
    }
    if(_netManager){
        delete _netManager;
        _netManager = nullptr;
    }
    if(_file){
        if(_file->isOpen())
            _file->close();
        delete _file;
        _file = nullptr;
    }
}

void WorkerDataCollections::StartCollection(int msec, QString filename, QString ip){

    if(!_collectedVariables.size()){
        emit Message("No variables selected!", MainWindow::ConsoleMessageType::Error);
        emit CollectionFinished();
        return;
    }

    if(_timer || _netManager || _file){
        emit Message("Collection already started!", MainWindow::ConsoleMessageType::Error);
        emit CollectionFinished();
        return;
    }

    try{
        _timer = new QTimer(this);
    }catch(...){
        if(_timer){
            delete _timer;
            _timer = nullptr;
        }
        emit Message("Failed to allocate memory for data collection", MainWindow::ConsoleMessageType::Error);
        emit CollectionFinished();
        return;
    }

    try{
        _netManager = new QNetworkAccessManager(this);
    }catch(...){
        if(_timer){
            delete _timer;
            _timer = nullptr;
        }
        if(_netManager){
            delete _netManager;
            _netManager = nullptr;
        }
        emit Message("Failed to allocate memory for data collection", MainWindow::ConsoleMessageType::Error);
        emit CollectionFinished();
        return;
    }

    try{
        _file = new QFile(filename + ".csv");
    }catch(...){
        if(_timer){
            delete _timer;
            _timer = nullptr;
        }
        if(_netManager){
            delete _netManager;
            _netManager = nullptr;
        }
        if(_file){
            delete _file;
            _file = nullptr;
        }
        emit Message("Failed to allocate memory for data collection", MainWindow::ConsoleMessageType::Error);
        emit CollectionFinished();
        return;
    }

    if(_collectedVariables.contains("REALCURRENTTIME")){
        try{
            _elapsedTimer = new QElapsedTimer();
        }catch(...){
            if(_timer){
                delete _timer;
                _timer = nullptr;
            }
            if(_elapsedTimer){
                delete _elapsedTimer;
                _elapsedTimer = nullptr;
            }
            if(_netManager){
                delete _netManager;
                _netManager = nullptr;
            }
            if(_file){
                delete _file;
                _file = nullptr;
            }
            emit Message("Failed to allocate memory for data collection", MainWindow::ConsoleMessageType::Error);
            emit CollectionFinished();
            return;
        }
        _elapsedTimer->start();
    }

    if(!_file->open(QIODevice::WriteOnly)){
        if(_timer){
            delete _timer;
            _timer = nullptr;
        }
        if(_elapsedTimer){
            delete _elapsedTimer;
            _elapsedTimer = nullptr;
        }
        if(_netManager){
            delete _netManager;
            _netManager = nullptr;
        }
        if(_file){
            delete _file;
            _file = nullptr;
        }
        emit Message("Failed to open file!", MainWindow::ConsoleMessageType::Error);
        emit CollectionFinished();
        return;
    }

    QTextStream out(_file);
    QMap<QString, QString>::Iterator itr = _collectedVariables.begin();

    out << itr.key();
    itr++;

    while(itr != _collectedVariables.end()){
        out << ";" << itr.key();
        itr++;
    }

    _file->close();

    connect(_timer, &QTimer::timeout, this, &WorkerDataCollections::Timeout);
    connect(this, &WorkerDataCollections::TimerStart, _timer, QOverload<int>::of(&QTimer::start));
    connect(this, &WorkerDataCollections::TimerStop, _timer, &QTimer::stop);

    connect(_netManager, &QNetworkAccessManager::finished, this, &WorkerDataCollections::ServerReply);

    _ip = ip;

    _timer->setTimerType(Qt::PreciseTimer);
    _timer->start(msec);
}

void WorkerDataCollections::GetVariable(QString variable){
    _netManager->get(QNetworkRequest(QUrl("http://" + _ip + "/?variable=" + variable)));
}

bool WorkerDataCollections::WriteData(){
    if(!_file)
        return false;

    if(!_file->open(QIODevice::Append))
        return false;

    QTextStream out(_file);
    QMap<QString, QString>::Iterator itr = _collectedVariables.begin();

    out << "\n" << itr.value();
    itr++;

    while(itr != _collectedVariables.end()){
        out << ";" << itr.value();
        itr++;
    }

    _file->close();
    return true;
}

void WorkerDataCollections::StopCollection(){
    disconnect(_netManager, &QNetworkAccessManager::finished, this, &WorkerDataCollections::ServerReply);
    emit TimerStop();
    emit CollectionFinished();
}

void WorkerDataCollections::Timeout(){
    GetVariable("WEBSERVER_BATCH_GET");
}

void WorkerDataCollections::ServerReply(QNetworkReply *reply){

    if(!reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()){
        emit Message("Webserver offline!", MainWindow::ConsoleMessageType::Error);
        reply->deleteLater();
        return StopCollection();
    }

    if(!reply->size()){
        reply->deleteLater();
        return;
    }

    QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object()["values"].toObject();
    QMap<QString,QString>::Iterator itr = _collectedVariables.begin();

    if(_collectedVariables.contains("REALCURRENTTIME")){
        _collectedVariables["REALCURRENTTIME"] = QString::number(_elapsedTimer->elapsed());
    }

    while(itr != _collectedVariables.end()){
        if(itr.key().compare("REALCURRENTTIME"))
            *itr = QString::number(json[itr.key()].toDouble());
        itr++;
    }

    if(!WriteData())
        emit Message("Data not stored!", MainWindow::ConsoleMessageType::Error);

    reply->deleteLater();
}

void WorkerDataCollections::AddVariable(QString variable){
    if(!_file)
        _collectedVariables.insert(variable, QString());
    else
        emit Message("Variable not inserted: " + variable);
}
