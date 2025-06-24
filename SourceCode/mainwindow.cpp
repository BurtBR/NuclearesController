#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QScrollBar>
#include "workerdatacollections.h"

#define _TIMERDIVIDER_X2 2
#define _TIMERDIVIDER_X3 3.529411764

const QMap<QString, MainWindow::NuclearVariable> MainWindow::_variablesNames = {
    {"REALCURRENTTIME", REALCURRENTTIME},
    {"TIME", TIME},
    {"CORE_TEMP", CORE_TEMP},
    {"CORE_STATE_CRITICALITY", CORE_STATE_CRITICALITY},
    {"CORE_IODINE_CUMULATIVE", CORE_IODINE_CUMULATIVE},
    {"CORE_XENON_CUMULATIVE", CORE_XENON_CUMULATIVE},
    {"COOLANT_SEC_0_VOLUME", COOLANT_SEC_0_VOLUME},
    {"COOLANT_SEC_1_VOLUME", COOLANT_SEC_1_VOLUME},
    {"COOLANT_SEC_2_VOLUME", COOLANT_SEC_2_VOLUME},
    {"STEAM_GEN_0_OUTLET", STEAM_GEN_0_OUTLET},
    {"STEAM_GEN_1_OUTLET", STEAM_GEN_1_OUTLET},
    {"STEAM_GEN_2_OUTLET", STEAM_GEN_2_OUTLET},
    {"COOLANT_SEC_CIRCULATION_PUMP_0_SPEED", COOLANT_SEC_CIRCULATION_PUMP_0_SPEED},
    {"COOLANT_SEC_CIRCULATION_PUMP_1_SPEED", COOLANT_SEC_CIRCULATION_PUMP_1_SPEED},
    {"COOLANT_SEC_CIRCULATION_PUMP_2_SPEED", COOLANT_SEC_CIRCULATION_PUMP_2_SPEED},
    {"COOLANT_SEC_CIRCULATION_PUMP_0_ORDERED_SPEED", COOLANT_SEC_CIRCULATION_PUMP_0_ORDERED_SPEED},
    {"COOLANT_SEC_CIRCULATION_PUMP_1_ORDERED_SPEED", COOLANT_SEC_CIRCULATION_PUMP_1_ORDERED_SPEED},
    {"COOLANT_SEC_CIRCULATION_PUMP_2_ORDERED_SPEED", COOLANT_SEC_CIRCULATION_PUMP_2_ORDERED_SPEED},
    {"POWER_DEMAND_MW", POWER_DEMAND_MW},
    {"COOLANT_CORE_FLOW_SPEED", COOLANT_CORE_FLOW_SPEED},
    {"COOLANT_CORE_FLOW_ORDERED_SPEED", COOLANT_CORE_FLOW_ORDERED_SPEED},
    {"GENERATOR_0_KW", GENERATOR_0_KW},
    {"GENERATOR_1_KW", GENERATOR_1_KW},
    {"GENERATOR_2_KW", GENERATOR_2_KW},
    {"GAME_SIM_SPEED", GAME_SIM_SPEED},
    {"RODS_POS_ACTUAL", RODS_POS_ACTUAL},
    {"RODS_POS_ORDERED", RODS_POS_ORDERED},
    {"ROD_BANK_POS_0_ORDERED", ROD_BANK_POS_0_ORDERED},
    {"STEAM_TURBINE_0_PRESSURE", STEAM_TURBINE_0_PRESSURE},
    {"STEAM_TURBINE_1_PRESSURE", STEAM_TURBINE_1_PRESSURE},
    {"STEAM_TURBINE_2_PRESSURE", STEAM_TURBINE_2_PRESSURE},
    {"STEAM_TURBINE_0_TEMPERATURE", STEAM_TURBINE_0_TEMPERATURE},
    {"STEAM_TURBINE_1_TEMPERATURE", STEAM_TURBINE_1_TEMPERATURE},
    {"STEAM_TURBINE_2_TEMPERATURE", STEAM_TURBINE_2_TEMPERATURE},
    {"WEBSERVER_BATCH_GET", WEBSERVER_BATCH_GET}
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), _ui(new Ui::MainWindow){
    _ui->setupUi(this);

    _netManager = new QNetworkAccessManager(this);

    _timer = new QTimer(this);
    _timer->setTimerType(Qt::PreciseTimer);
    connect(_timer, &QTimer::timeout, this, &MainWindow::Timeout);
    connect(this, &MainWindow::TimerStart, _timer, QOverload<int>::of(&QTimer::start));
    connect(this, &MainWindow::TimerStop, _timer, &QTimer::stop);

    connect(_ui->buttonStart, &QPushButton::clicked, this, &MainWindow::On_buttonStart_Clicked);
    connect(_ui->buttonStartDataCollection, &QPushButton::clicked, this, &MainWindow::On_buttonStartDataCollection_Clicked);

    connect(_ui->spinRodKp, &QDoubleSpinBox::editingFinished, this, &MainWindow::On_spinRodKp_editingFinished);
    connect(_ui->spinRodKi, &QDoubleSpinBox::editingFinished, this, &MainWindow::On_spinRodKi_editingFinished);
    connect(_ui->spinRodKd, &QDoubleSpinBox::editingFinished, this, &MainWindow::On_spinRodKd_editingFinished);
    connect(_ui->spinIntegralMaximumRods, &QDoubleSpinBox::editingFinished, this, &MainWindow::On_spinIntegralMaximumRods_editingFinished);
    On_spinRodKp_editingFinished();
    On_spinRodKi_editingFinished();
    On_spinRodKd_editingFinished();
    On_spinIntegralMaximumRods_editingFinished();


    connect(_ui->spinSteamKd, &QDoubleSpinBox::editingFinished, this, &MainWindow::On_spinSteamKd_editingFinished);
    connect(_ui->spinSteamKi, &QDoubleSpinBox::editingFinished, this, &MainWindow::On_spinSteamKi_editingFinished);
    connect(_ui->spinSteamKp, &QDoubleSpinBox::editingFinished, this, &MainWindow::On_spinSteamKp_editingFinished);
    connect(_ui->spinIntegralMaximumGen, &QDoubleSpinBox::editingFinished, this, &MainWindow::On_spinIntegralMaximumGen_editingFinished);
    On_spinSteamKp_editingFinished();
    On_spinSteamKi_editingFinished();
    On_spinSteamKd_editingFinished();
    On_spinIntegralMaximumGen_editingFinished();

    connect(_ui->spinEnergyKp, &QDoubleSpinBox::editingFinished, this, &MainWindow::On_spinEnergyKp_editingFinished);
    connect(_ui->spinEnergyKi, &QDoubleSpinBox::editingFinished, this, &MainWindow::On_spinEnergyKi_editingFinished);
    connect(_ui->spinEnergyKd, &QDoubleSpinBox::editingFinished, this, &MainWindow::On_spinEnergyKd_editingFinished);
    connect(_ui->spinIntegralMaximumCorePump, &QDoubleSpinBox::editingFinished, this, &MainWindow::On_spinIntegralMaximumCorePump_editingFinished);
    On_spinEnergyKp_editingFinished();
    On_spinEnergyKi_editingFinished();
    On_spinEnergyKd_editingFinished();
    On_spinIntegralMaximumCorePump_editingFinished();


    connect(_ui->spinPressureKp, &QDoubleSpinBox::editingFinished, this, &MainWindow::On_spinPressureKp_editingFinihed);
    connect(_ui->spinPressureKi, &QDoubleSpinBox::editingFinished, this, &MainWindow::On_spinPressureKi_editingFinihed);
    connect(_ui->spinPressureKd, &QDoubleSpinBox::editingFinished, this, &MainWindow::On_spinPressureKd_editingFinihed);
    connect(_ui->spinPressureMaxIntegral, &QDoubleSpinBox::editingFinished, this, &MainWindow::On_spinPressureMaxIntegral_editingFinished);
    On_spinPressureKp_editingFinihed();
    On_spinPressureKi_editingFinihed();
    On_spinPressureKd_editingFinihed();
    On_spinPressureMaxIntegral_editingFinished();

    // TEMPORARILY HIDDEN
    _ui->tabWidget->removeTab(3);
}

MainWindow::~MainWindow(){

    DeleteThread(&_threadDataCollection);

    if(_netManager){
        disconnect(_netManager, &QNetworkAccessManager::finished, this, &MainWindow::ReplyReceived);
        delete _netManager;
        _netManager = nullptr;
    }

    if(_timer){
        disconnect(_timer, &QTimer::timeout, this, &MainWindow::Timeout);
        disconnect(this, &MainWindow::TimerStart, _timer, QOverload<int>::of(&QTimer::start));
        disconnect(this, &MainWindow::TimerStop, _timer, &QTimer::stop);
        delete _timer;
        _timer = nullptr;
    }

    delete _ui;
}

void MainWindow::DeleteThread(QThread **thread){

    if(!thread)
        return;

    QThread *t = *thread;
    if(t){
        t->quit();
        if(!t->wait(2000)){
            t->terminate();
            t->wait();
        }
        if(*thread){
            delete *thread;
            *thread = nullptr;
        }
    }
}

void MainWindow::ConsoleMessage(QString msg, ConsoleMessageType type){
    switch(type){
    case ConsoleMessageType::Error:
        msg.prepend("<span style=\"color:#ff5050;\">ERROR: ");
        msg.append("</span>");
        break;
    case ConsoleMessageType::Warning:
        msg.prepend("<span style=\"color:#ffff30;\">WARNING: ");
        msg.append("</span>");
        break;
    default:
        break;
    }

    _ui->textConsole->append(QDateTime::currentDateTime().toString("hh:mm:ss") + " " + msg);
    _ui->textConsole->verticalScrollBar()->setValue(_ui->textConsole->verticalScrollBar()->maximum());
}

void MainWindow::OrderRods(double rodPosition, double coreTemp){
    double pidResult = _rodPID.Calculate(coreTemp - _ui->spinTemperatureTarget->value());
    double newHeight = (rodPosition + pidResult);

    if(newHeight > _ui->spinRodMax->value()){
        newHeight = _ui->spinRodMax->value();
        _rodPID.ResetSum();
    }else if(newHeight < _ui->spinRodMin->value()){
        newHeight = _ui->spinRodMin->value();
        _rodPID.ResetSum();
    }

    if(newHeight != rodPosition){
        PostRequest("RODS_ALL_POS_ORDERED", QString::number(newHeight));
    }

    SetCoreTemperatureButtonGreen();
    _ui->lcdPID_TempP->display(_rodPID.GetP());
    _ui->lcdPID_TempI->display(_rodPID.GetI());
    _ui->lcdPID_TempD->display(_rodPID.GetD());
}

void MainWindow::OrderCorePump(){
    int newflow = qRound(((double)_corePump) + _energyPID.Calculate(_requiredPower-_generatedPower));

    if(newflow < _ui->spinCorePumpMin->value()){
        newflow = _ui->spinCorePumpMin->value();
        _energyPID.ResetSum();
    }else if(newflow > _ui->spinCorePumpMax->value()){
        newflow = _ui->spinCorePumpMax->value();
        _energyPID.ResetSum();
    }

    if(newflow != _corePump){
        QString newflowstr = QString::number(newflow);
        _corePump = newflow;

        PostRequest("COOLANT_CORE_CIRCULATION_PUMP_0_ORDERED_SPEED", newflowstr);
        PostRequest("COOLANT_CORE_CIRCULATION_PUMP_1_ORDERED_SPEED", newflowstr);
        PostRequest("COOLANT_CORE_CIRCULATION_PUMP_2_ORDERED_SPEED", newflowstr);
    }

    SetPowerGenButtonGreen();
    _ui->lcdPID_PowerP->display(_energyPID.GetP());
    _ui->lcdPID_PowerI->display(_energyPID.GetI());
    _ui->lcdPID_PowerD->display(_energyPID.GetD());
}

void MainWindow::OrderSteamFlow(int number){
    int newflow;
    int error = _ui->spinGenVolumeTarget->value()-_coolantVolume[number];

    double command = _steamPID[number].Calculate(error);

    newflow = qRound(command + _secCoolFlow[number]);

    if(newflow < _ui->spinSteamFlowMin->value()){
        newflow = _ui->spinSteamFlowMin->value();
        _steamPID[number].ResetSum();
    }else if(newflow > _ui->spinSteamFlowMax->value()){
        newflow = _ui->spinSteamFlowMax->value();
        _steamPID[number].ResetSum();
    }

    if(newflow != _secCoolFlow[number]){
        _secCoolFlow[number] = newflow;
        PostRequest("COOLANT_SEC_CIRCULATION_PUMP_"+QString::number(number)+"_ORDERED_SPEED", QString::number(newflow));
    }

    SetSteamButtonGreen(number);

    switch(number){
    case 0:
        _ui->lcdPID_L1_SteamP->display(_steamPID[0].GetP());
        _ui->lcdPID_L1_SteamI->display(_steamPID[0].GetI());
        _ui->lcdPID_L1_SteamD->display(_steamPID[0].GetD());
        break;
    case 1:
        _ui->lcdPID_L2_SteamP->display(_steamPID[1].GetP());
        _ui->lcdPID_L2_SteamI->display(_steamPID[1].GetI());
        _ui->lcdPID_L2_SteamD->display(_steamPID[1].GetD());
        break;
    case 2:
        _ui->lcdPID_L3_SteamP->display(_steamPID[2].GetP());
        _ui->lcdPID_L3_SteamI->display(_steamPID[2].GetI());
        _ui->lcdPID_L3_SteamD->display(_steamPID[2].GetD());
        break;
    default:
        break;
    }
}

void MainWindow::OrderPressure(int number, double valve, double pressure){
    double pidResult = _pressurePID[number].Calculate(pressure - _ui->spinPressureTarget->value());
    double newPosition = valve + pidResult;

    if(newPosition > _ui->spinPressureValveMax->value()){
        newPosition = _ui->spinPressureValveMax->value();
        _pressurePID[number].ResetSum();
    }else if(newPosition < _ui->spinPressureValveMin->value()){
        newPosition = _ui->spinPressureValveMin->value();
        _pressurePID[number].ResetSum();
    }

    if(newPosition != valve){
        PostRequest("MSCV_" + QString::number(number) + "_OPENING_ORDERED", QString::number(newPosition));
    }

    SetPressureButtonGreen(number);

    switch(number){
    case 0:
        _ui->lcd_L1_Pres_P->display(_pressurePID[number].GetP());
        _ui->lcd_L1_Pres_I->display(_pressurePID[number].GetI());
        _ui->lcd_L1_Pres_D->display(_pressurePID[number].GetD());
        break;
    case 1:
        _ui->lcd_L2_Pres_P->display(_pressurePID[number].GetP());
        _ui->lcd_L2_Pres_I->display(_pressurePID[number].GetI());
        _ui->lcd_L2_Pres_D->display(_pressurePID[number].GetD());
        break;
    case 2:
        _ui->lcd_L3_Pres_P->display(_pressurePID[number].GetP());
        _ui->lcd_L3_Pres_I->display(_pressurePID[number].GetI());
        _ui->lcd_L3_Pres_D->display(_pressurePID[number].GetD());
        break;
    default:
        break;
    }
}

void MainWindow::SetSteamButtonRed(int number){
    switch(number){
    case 0:
        _ui->widget_status_L1->setStyleSheet("background-color: rgb(255, 100, 100)");
        break;
    case 1:
        _ui->widget_status_L2->setStyleSheet("background-color: rgb(255, 100, 100)");
        break;
    case 2:
        _ui->widget_status_L3->setStyleSheet("background-color: rgb(255, 100, 100)");
        break;
    default:
        break;
    }
}

void MainWindow::SetSteamButtonGreen(int number){
    switch(number){
    case 0:
        _ui->widget_status_L1->setStyleSheet("background-color: rgb(100, 255, 100)");
        break;
    case 1:
        _ui->widget_status_L2->setStyleSheet("background-color: rgb(100, 255, 100)");
        break;
    case 2:
        _ui->widget_status_L3->setStyleSheet("background-color: rgb(100, 255, 100)");
        break;
    default:
        break;
    }
}

void MainWindow::SetPressureButtonRed(int number){
    switch(number){
    case 0:
        _ui->widget_status_Pressure_L1->setStyleSheet("background-color: rgb(255, 100, 100)");
        break;
    case 1:
        _ui->widget_status_Pressure_L2->setStyleSheet("background-color: rgb(255, 100, 100)");
        break;
    case 2:
        _ui->widget_status_Pressure_L3->setStyleSheet("background-color: rgb(255, 100, 100)");
        break;
    default:
        break;
    }
}

void MainWindow::SetPressureButtonGreen(int number){
    switch(number){
    case 0:
        _ui->widget_status_Pressure_L1->setStyleSheet("background-color: rgb(100, 255, 100)");
        break;
    case 1:
        _ui->widget_status_Pressure_L2->setStyleSheet("background-color: rgb(100, 255, 100)");
        break;
    case 2:
        _ui->widget_status_Pressure_L3->setStyleSheet("background-color: rgb(100, 255, 100)");
        break;
    default:
        break;
    }
}

void MainWindow::SetPowerGenButtonRed(){
    _ui->widget_status_powerGen->setStyleSheet("background-color: rgb(255, 100, 100)");
}

void MainWindow::SetPowerGenButtonGreen(){
    _ui->widget_status_powerGen->setStyleSheet("background-color: rgb(100, 255, 100)");
}

void MainWindow::SetCoreTemperatureButtonRed(){
    _ui->widgetStatusCoreTemp->setStyleSheet("background-color: rgb(255, 100, 100)");
}

void MainWindow::SetCoreTemperatureButtonGreen(){
    _ui->widgetStatusCoreTemp->setStyleSheet("background-color: rgb(100, 255, 100)");
}

void MainWindow::StartController(){
    _ui->buttonStart->setText("Stop");
    connect(_netManager, &QNetworkAccessManager::finished, this, &MainWindow::ReplyReceived);
    ConsoleMessage("Controller started");
    //ConsoleMessage("PAUSING THE GAME WILL ACCUMULATE THE ERROR. RESET THE CONTROLLER BEFORE RESUME!", ConsoleMessageType::Warning);
    switch(_lastGameSpeed){
    case 1:
        emit TimerStart(_ui->spinPeriod->value());
        break;
    case 2:
        emit TimerStart(_ui->spinPeriod->value()/_TIMERDIVIDER_X2);
        break;
    case 4:
        emit TimerStart(_ui->spinPeriod->value()/_TIMERDIVIDER_X3);
        break;
    default:
        break;
    }
}

void MainWindow::StopController(){
    emit TimerStop();
    disconnect(_netManager, &QNetworkAccessManager::finished, this, &MainWindow::ReplyReceived);
    _ui->buttonStart->setText("Start");
    ConsoleMessage("Controller stopped");
    _steamPID[0].Reset();
    _steamPID[1].Reset();
    _steamPID[2].Reset();
    _pressurePID[0].Reset();
    _pressurePID[1].Reset();
    _pressurePID[2].Reset();
    _energyPID.Reset();
    _rodPID.Reset();
    SetSteamButtonRed(0);
    SetSteamButtonRed(1);
    SetSteamButtonRed(2);
    SetPowerGenButtonRed();
    SetCoreTemperatureButtonRed();
    SetPressureButtonRed(0);
    SetPressureButtonRed(1);
    SetPressureButtonRed(2);
}

void MainWindow::PostRequest(QString variable, QString value){
    QNetworkRequest request;
    QUrl url("http://" + _ui->lineIP->text() + "/?variable="+ variable +"&value=" + value);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    request.setUrl(url);
    _netManager->post(request, QByteArray());
}

void MainWindow::GetRequest(QString variable){
    _netManager->get(QNetworkRequest(QUrl("http://" + _ui->lineIP->text() + "/?variable=" + variable)));
}

void MainWindow::CollectionFinished(){
    DeleteThread(&_threadDataCollection);
    _ui->buttonStartDataCollection->setText("Start Data Collection");
    ConsoleMessage("Data collection finished");
}

void MainWindow::On_buttonStart_Clicked(){
    if(_timer){
        if(_timer->isActive())
            StopController();
        else
            StartController();
    }
}

void MainWindow::On_buttonStartDataCollection_Clicked(){
    if(_threadDataCollection){
        emit StopCollection();
    }else{

        WorkerDataCollections *worker;

        try{
            worker = new WorkerDataCollections();
        }catch(...){
            if(worker)
                delete worker;
            ConsoleMessage("Failed to allocate memory!", ConsoleMessageType::Error);
            return;
        }

        try{
            _threadDataCollection = new QThread(this);
        }catch(...){
            if(_threadDataCollection){
                delete _threadDataCollection;
                _threadDataCollection = nullptr;
            }
            delete worker;
            ConsoleMessage("Failed to allocate memory!", ConsoleMessageType::Error);
            return;
        }

        if(_ui->checkCollectRealtime->isChecked())
            worker->AddVariable("REALCURRENTTIME");

        if(_ui->checkCollectCoreTemperature->isChecked())
            worker->AddVariable("CORE_TEMP");

        if(_ui->checkCollectRodsCommanded->isChecked())
            worker->AddVariable("ROD_BANK_POS_0_ORDERED");

        if(_ui->checkCollectRodsPosition->isChecked())
            worker->AddVariable("RODS_POS_ACTUAL");

        if(_ui->checkCollectCumulativeIodine->isChecked())
            worker->AddVariable("CORE_IODINE_CUMULATIVE");

        if(_ui->checkCollectCumulativeXenon->isChecked())
            worker->AddVariable("CORE_XENON_CUMULATIVE");

        connect(_threadDataCollection, &QThread::finished, worker, &WorkerDataCollections::deleteLater);

        connect(worker, &WorkerDataCollections::CollectionFinished, this, &MainWindow::CollectionFinished);
        connect(worker, &WorkerDataCollections::Message, this, &MainWindow::ConsoleMessage);

        connect(this, &MainWindow::StartCollection, worker, &WorkerDataCollections::StartCollection);
        connect(this, &MainWindow::StopCollection, worker, &WorkerDataCollections::StopCollection);

        worker->moveToThread(_threadDataCollection);
        _threadDataCollection->start();

        _ui->buttonStartDataCollection->setText("Stop Data Collection");
        ConsoleMessage("Data collection started");
        emit StartCollection(_ui->spinCollectionPeriod->value(), _ui->lineCollectionFilename->text(), _ui->lineIP->text());
    }
}

void MainWindow::On_spinPeriod_editingFinished(){
    if(_timer){
        if(_timer->isActive()){
            StopController();
            StartController();
        }
    }
}

void MainWindow::On_spinRodKp_editingFinished(){
    _rodPID.SetKp(_ui->spinRodKp->value());
}

void MainWindow::On_spinRodKi_editingFinished(){
    _rodPID.SetKi(_ui->spinRodKi->value());
}

void MainWindow::On_spinRodKd_editingFinished(){
    _rodPID.SetKd(_ui->spinRodKd->value());
}

void MainWindow::On_spinSteamKp_editingFinished(){
    double value = _ui->spinSteamKp->value();
    _steamPID[0].SetKp(value);
    _steamPID[1].SetKp(value);
    _steamPID[2].SetKp(value);
}

void MainWindow::On_spinSteamKi_editingFinished(){
    double value = _ui->spinSteamKi->value();
    _steamPID[0].SetKi(value);
    _steamPID[1].SetKi(value);
    _steamPID[2].SetKi(value);
}

void MainWindow::On_spinSteamKd_editingFinished(){
    double value = _ui->spinSteamKd->value();
    _steamPID[0].SetKd(value);
    _steamPID[1].SetKd(value);
    _steamPID[2].SetKd(value);
}

void MainWindow::On_spinPressureKp_editingFinihed(){
    _pressurePID[0].SetKp(_ui->spinPressureKp->value());
    _pressurePID[1].SetKp(_ui->spinPressureKp->value());
    _pressurePID[2].SetKp(_ui->spinPressureKp->value());
}

void MainWindow::On_spinPressureKi_editingFinihed(){
    _pressurePID[0].SetKi(_ui->spinPressureKi->value());
    _pressurePID[1].SetKi(_ui->spinPressureKi->value());
    _pressurePID[2].SetKi(_ui->spinPressureKi->value());
}

void MainWindow::On_spinPressureKd_editingFinihed(){
    _pressurePID[0].SetKd(_ui->spinPressureKd->value());
    _pressurePID[1].SetKd(_ui->spinPressureKd->value());
    _pressurePID[2].SetKd(_ui->spinPressureKd->value());
}

void MainWindow::On_spinEnergyKp_editingFinished(){
    _energyPID.SetKp(_ui->spinEnergyKp->value());
}

void MainWindow::On_spinEnergyKi_editingFinished(){
    _energyPID.SetKi(_ui->spinEnergyKi->value());
}

void MainWindow::On_spinEnergyKd_editingFinished(){
    _energyPID.SetKd(_ui->spinEnergyKd->value());
}

void MainWindow::On_spinIntegralMaximumGen_editingFinished(){
    double value = _ui->spinIntegralMaximumGen->value();
    _steamPID[0].SetIntegralLimit(value);
    _steamPID[1].SetIntegralLimit(value);
    _steamPID[2].SetIntegralLimit(value);
}

void MainWindow::On_spinIntegralMaximumCorePump_editingFinished(){
    _energyPID.SetIntegralLimit(_ui->spinIntegralMaximumCorePump->value());
}

void MainWindow::On_spinIntegralMaximumRods_editingFinished(){
    _rodPID.SetIntegralLimit(_ui->spinIntegralMaximumRods->value());
}

void MainWindow::On_spinPressureMaxIntegral_editingFinished(){
    _pressurePID[0].SetIntegralLimit(_ui->spinPressureMaxIntegral->value());
    _pressurePID[1].SetIntegralLimit(_ui->spinPressureMaxIntegral->value());
    _pressurePID[2].SetIntegralLimit(_ui->spinPressureMaxIntegral->value());
}

void MainWindow::On_checkEnableSteam_checkStatusChanged(Qt::CheckState state){
    if(!state){
        SetSteamButtonRed(0);
        SetSteamButtonRed(1);
        SetSteamButtonRed(2);
    }
}

void MainWindow::On_checkEnableEnergyPID_checkStatusChanged(Qt::CheckState state){
    if(!state){
        SetPowerGenButtonRed();
    }
}

void MainWindow::On_checkEnableRodControl_checkStatusChanged(Qt::CheckState state){
    if(!state){
        SetCoreTemperatureButtonRed();
    }
}

void MainWindow::On_checkEnablePressureController_checkedStatusChanged(Qt::CheckState state){
    if(!state){
        SetPressureButtonRed(0);
        SetPressureButtonRed(1);
        SetPressureButtonRed(2);
    }
}

void MainWindow::ReplyReceived(QNetworkReply *reply){

    if(!reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()){
        StopController();
        ConsoleMessage("Webserver offline!", ConsoleMessageType::Error);
        reply->deleteLater();
        return;
    }

    if(!reply->size()){
        reply->deleteLater();
        return;
    }

    QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object()["values"].toObject();
    double actuator, sensor;

    _ui->labelTime->setText(json["TIME"].toString());

    if(_ui->checkEnableRodControl->isChecked()){
        sensor = json["CORE_TEMP"].toDouble();
        _ui->lcdCoreTemp->display(sensor);
        actuator = json["RODS_POS_ACTUAL"].toDouble();
        _ui->lcdRodPosition->display(actuator);
        _ui->lcdReactivity->display(json["CORE_STATE_CRITICALITY"].toDouble());
        OrderRods(actuator, sensor);
    }

    if(_ui->checkEnablePressureController->isChecked()){
        if(json["STEAM_GEN_0_STATUS"].toInt() ==2){
            sensor = json["STEAM_TURBINE_0_PRESSURE"].toDouble();
            actuator = json["MSCV_0_OPENING_ACTUAL"].toDouble();
            _ui->lcd_L1_Pres_Temp->display(json["STEAM_TURBINE_0_TEMPERATURE"].toDouble());
            _ui->lcd_L1_Pres_Valve->display(actuator);
            _ui->lcd_L1_Pres_Pres->display(sensor);
            OrderPressure(0, actuator, sensor);
        }

        if(json["STEAM_GEN_1_STATUS"].toInt() ==2){
            actuator = json["MSCV_1_OPENING_ACTUAL"].toDouble();
            sensor = json["STEAM_TURBINE_1_PRESSURE"].toDouble();
            _ui->lcd_L2_Pres_Temp->display(json["STEAM_TURBINE_1_TEMPERATURE"].toDouble());
            _ui->lcd_L2_Pres_Valve->display(actuator);
            _ui->lcd_L2_Pres_Pres->display(sensor);
            OrderPressure(1, actuator, sensor);
        }

        if(json["STEAM_GEN_2_STATUS"].toInt() ==2){
            actuator = json["MSCV_2_OPENING_ACTUAL"].toDouble();
            sensor = json["STEAM_TURBINE_2_PRESSURE"].toDouble();
            _ui->lcd_L3_Pres_Temp->display(json["STEAM_TURBINE_2_TEMPERATURE"].toDouble());
            _ui->lcd_L3_Pres_Valve->display(actuator);
            _ui->lcd_L3_Pres_Pres->display(sensor);
            OrderPressure(2, actuator, sensor);
        }
    }

    reply->deleteLater();
}

void MainWindow::Timeout(){
    GetRequest("WEBSERVER_BATCH_GET");
}
