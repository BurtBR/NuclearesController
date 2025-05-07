#include "workerdatacollections.h"

WorkerDataCollections::WorkerDataCollections(QObject *parent) : QObject{parent}{

}

WorkerDataCollections::~WorkerDataCollections(){
    if(_timer){
        delete _timer;
        _timer = nullptr;
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

    if(!_file->open(QIODevice::WriteOnly)){
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
        emit Message("Failed to open file!", MainWindow::ConsoleMessageType::Error);
        emit CollectionFinished();
        return;
    }

    QTextStream out(_file);
    QMap<MainWindow::NuclearVariable, QByteArray>::Iterator itr = _collectedVariables.begin();

    out << MainWindow::_variablesNames.key(itr.key());
    itr++;

    while(itr != _collectedVariables.end()){
        out << ";" << MainWindow::_variablesNames.key(itr.key());
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

void WorkerDataCollections::GetVariable(MainWindow::NuclearVariable variable){
    _netManager->get(QNetworkRequest(QUrl("http://" + _ip + "/?variable=" + MainWindow::_variablesNames.key(variable))));
}

bool WorkerDataCollections::WriteData(){
    if(!_file)
        return false;

    if(!_file->open(QIODevice::ReadWrite))
        return false;

    QTextStream out(_file);
    QMap<MainWindow::NuclearVariable, QByteArray>::Iterator itr = _collectedVariables.begin();

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
    QMap<MainWindow::NuclearVariable, QByteArray>::Iterator itr = _collectedVariables.begin();

    while(itr != _collectedVariables.end()){
        GetVariable(itr.key());
        itr++;
    }
}

void WorkerDataCollections::ServerReply(QNetworkReply *reply){

    if(!reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()){
        emit Message("Webserver offline!", MainWindow::ConsoleMessageType::Error);
        reply->deleteLater();
        return StopCollection();
    }

    QStringList query = reply->url().query().split('=');
    MainWindow::NuclearVariable variable;

    if(query.size() != 2){
        reply->deleteLater();
        return;
    }else if(!MainWindow::_variablesNames.contains(query.at(1))){
        reply->deleteLater();
        return;
    }

    variable = MainWindow::_variablesNames.value(query.at(1));

    if(!_collectedVariables.contains(variable)){
        reply->deleteLater();
        return;
    }

    _qtyReceived++;

    _collectedVariables[variable] = reply->readAll();

    if(_qtyReceived == _collectedVariables.size()){
        if(!WriteData())
            emit Message("Data not stored!", MainWindow::ConsoleMessageType::Error);
        _qtyReceived = 0;
    }

    reply->deleteLater();
}

void WorkerDataCollections::AddVariable(MainWindow::NuclearVariable variable){
    if(!_file)
        _collectedVariables.insert(variable, QByteArray());
    else
        emit Message("Variable not inserted: " + MainWindow::_variablesNames.key(variable));
}
