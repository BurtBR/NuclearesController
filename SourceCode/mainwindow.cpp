#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QScrollBar>
#include "workerdatacollections.h"

const QString MainWindow::_settingsFilename = "AppSettings.ini";

const QMap<MainWindow::ConfigName, QString> MainWindow::_configNames = {
    {ConfigName::Ip, "General/Ip"},
    {ConfigName::SamplingPeriod, "General/SamplingPeriod"},
    {ConfigName::SteamGenFlowMin, "SteamGenerator/SteamGenFlowMin"},
    {ConfigName::SteamGenFlowMax, "SteamGenerator/SteamGenFlowMax"},
    {ConfigName::SteamGenKp, "SteamGenerator/SteamGenKp"},
    {ConfigName::SteamGenKi, "SteamGenerator/SteamGenKi"},
    {ConfigName::SteamGenKd, "SteamGenerator/SteamGenKd"},
    {ConfigName::SteamGenIntegralMax, "SteamGenerator/SteamGenIntegralMax"},
    {ConfigName::SteamGenVolumeTarget, "SteamGenerator/SteamGenVolumeTarget"},
    {ConfigName::SteamGenEnabled, "SteamGenerator/SteamGenEnabled"},
    {ConfigName::SteamPressureValveMin, "SteamPressure/SteamPressureValveMin"},
    {ConfigName::SteamPressureValveMax, "SteamPressure/SteamPressureValveMax"},
    {ConfigName::SteamPressureKp, "SteamPressure/SteamPressureKp"},
    {ConfigName::SteamPressureKi, "SteamPressure/SteamPressureKi"},
    {ConfigName::SteamPressureKd, "SteamPressure/SteamPressureKd"},
    {ConfigName::SteamPressureIntegralMax, "SteamPressure/SteamPressureIntegralMax"},
    {ConfigName::SteamPressureTarget, "SteamPressure/SteamPressureTarget"},
    {ConfigName::SteamPressureEnabled, "SteamPressure/SteamPressureEnabled"},
    {ConfigName::PowerGenFlowMin, "PowerGeneration/PowerGenFlowMin"},
    {ConfigName::PowerGenFlowMax, "PowerGeneration/PowerGenFlowMax"},
    {ConfigName::PowerGenKp, "PowerGeneration/PowerGenKp"},
    {ConfigName::PowerGenKi, "PowerGeneration/PowerGenKi"},
    {ConfigName::PowerGenKd, "PowerGeneration/PowerGenKd"},
    {ConfigName::PowerGenIntegralMax, "PowerGeneration/PowerGenIntegralMax"},
    {ConfigName::PowerGenEnabled, "PowerGeneration/PowerGenEnabled"},
    {ConfigName::CoreTempRodMin, "CoreTemperature/CoreTempRodMin"},
    {ConfigName::CoreTempRodMax, "CoreTemperature/CoreTempRodMax"},
    {ConfigName::CoreTempKp, "CoreTemperature/CoreTempKp"},
    {ConfigName::CoreTempKi, "CoreTemperature/CoreTempKi"},
    {ConfigName::CoreTempKd, "CoreTemperature/CoreTempKd"},
    {ConfigName::CoreTempIntegralMax, "CoreTemperature/CoreTempIntegralMax"},
    {ConfigName::CoreTempTarget, "CoreTemperature/CoreTempTarget"},
    {ConfigName::CoreTempEnabled, "CoreTemperature/CoreTempEnabled"}
} ;

#define _TIMERDIVIDER_X2 2
#define _TIMERDIVIDER_X3 3.529411764

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), _ui(new Ui::MainWindow){
    _ui->setupUi(this);

    // Temporarily hidden
    _ui->checkWorkX1->hide();
    _ui->checkWorkX2->hide();
    _ui->checkWorkX3->hide();

    _netManager = new QNetworkAccessManager(this);

    _timer = new QTimer(this);
    _timer->setTimerType(Qt::PreciseTimer);
    connect(_timer, &QTimer::timeout, this, &MainWindow::Timeout);
    connect(this, &MainWindow::TimerStart, _timer, QOverload<int>::of(&QTimer::start));
    connect(this, &MainWindow::TimerStop, _timer, &QTimer::stop);

    connect(_ui->buttonStart, &QPushButton::clicked, this, &MainWindow::On_buttonStart_Clicked);
    connect(_ui->buttonStartDataCollection, &QPushButton::clicked, this, &MainWindow::On_buttonStartDataCollection_Clicked);

    connect(_ui->spinPeriod, &QSpinBox::editingFinished, this, &MainWindow::On_spinPeriod_editingFinished);

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

    GetConfigs();
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


    StoreConfigs();

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

void MainWindow::OrderCorePump(double pump, double generated, double required){
    double pidResult = _energyPID.Calculate(required-generated);
    double newflow = pump+pidResult;

    if(newflow < _ui->spinCorePumpMin->value()){
        newflow = _ui->spinCorePumpMin->value();
        _energyPID.ResetSum();
    }else if(newflow > _ui->spinCorePumpMax->value()){
        newflow = _ui->spinCorePumpMax->value();
        _energyPID.ResetSum();
    }

    if(newflow != pump){
        QString newflowstr = QString::number(newflow);

        PostRequest("COOLANT_CORE_CIRCULATION_PUMP_0_ORDERED_SPEED", newflowstr);
        PostRequest("COOLANT_CORE_CIRCULATION_PUMP_1_ORDERED_SPEED", newflowstr);
        PostRequest("COOLANT_CORE_CIRCULATION_PUMP_2_ORDERED_SPEED", newflowstr);
    }

    SetPowerGenButtonGreen();
    _ui->lcdPID_PowerP->display(_energyPID.GetP());
    _ui->lcdPID_PowerI->display(_energyPID.GetI());
    _ui->lcdPID_PowerD->display(_energyPID.GetD());
}

void MainWindow::OrderSteamFlow(int number, double valve, double volume){
    double pidResult = _steamPID[number].Calculate(_ui->spinGenVolumeTarget->value() - volume);
    int newflow = valve + pidResult;

    if(newflow < _ui->spinSteamFlowMin->value()){
        newflow = _ui->spinSteamFlowMin->value();
        _steamPID[number].ResetSum();
    }else if(newflow > _ui->spinSteamFlowMax->value()){
        newflow = _ui->spinSteamFlowMax->value();
        _steamPID[number].ResetSum();
    }

    if(newflow != valve){
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
    emit TimerStart(_ui->spinPeriod->value());
    // switch(_lastGameSpeed){
    // case 1:
    //     emit TimerStart(_ui->spinPeriod->value());
    //     break;
    // case 2:
    //     emit TimerStart(_ui->spinPeriod->value()/_TIMERDIVIDER_X2);
    //     break;
    // case 4:
    //     emit TimerStart(_ui->spinPeriod->value()/_TIMERDIVIDER_X3);
    //     break;
    // default:
    //     break;
    // }
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

void MainWindow::GetConfigs(){
    QSettings settings = QSettings(_settingsFilename, QSettings::Format::IniFormat);

    if(settings.value(_configNames[ConfigName::Ip]).isNull())
        return;

    _ui->lineIP->setText(settings.value(_configNames[ConfigName::Ip]).toString());
    _ui->spinPeriod->setValue(settings.value(_configNames[ConfigName::SamplingPeriod]).toInt());

    _ui->spinSteamFlowMin->setValue(settings.value(_configNames[ConfigName::SteamGenFlowMin]).toDouble());
    _ui->spinSteamFlowMax->setValue(settings.value(_configNames[ConfigName::SteamGenFlowMax]).toDouble());
    _ui->spinSteamKp->setValue(settings.value(_configNames[ConfigName::SteamGenKp]).toDouble());
    _ui->spinSteamKi->setValue(settings.value(_configNames[ConfigName::SteamGenKi]).toDouble());
    _ui->spinSteamKd->setValue(settings.value(_configNames[ConfigName::SteamGenKd]).toDouble());
    _ui->spinIntegralMaximumGen->setValue(settings.value(_configNames[ConfigName::SteamGenIntegralMax]).toDouble());
    _ui->spinGenVolumeTarget->setValue(settings.value(_configNames[ConfigName::SteamGenVolumeTarget]).toDouble());
    _ui->checkEnableSteam->setChecked(settings.value(_configNames[ConfigName::SteamGenEnabled]).toBool());

    _ui->spinPressureValveMin->setValue(settings.value(_configNames[ConfigName::SteamPressureValveMin]).toDouble());
    _ui->spinPressureValveMax->setValue(settings.value(_configNames[ConfigName::SteamPressureValveMax]).toDouble());
    _ui->spinPressureKp->setValue(settings.value(_configNames[ConfigName::SteamPressureKp]).toDouble());
    _ui->spinPressureKi->setValue(settings.value(_configNames[ConfigName::SteamPressureKi]).toDouble());
    _ui->spinPressureKd->setValue(settings.value(_configNames[ConfigName::SteamPressureKd]).toDouble());
    _ui->spinPressureMaxIntegral->setValue(settings.value(_configNames[ConfigName::SteamPressureIntegralMax]).toDouble());
    _ui->spinPressureTarget->setValue(settings.value(_configNames[ConfigName::SteamPressureTarget]).toDouble());
    _ui->checkEnablePressureController->setChecked(settings.value(_configNames[ConfigName::SteamPressureEnabled]).toBool());

    _ui->spinCorePumpMin->setValue(settings.value(_configNames[ConfigName::PowerGenFlowMin]).toDouble());
    _ui->spinCorePumpMax->setValue(settings.value(_configNames[ConfigName::PowerGenFlowMax]).toDouble());
    _ui->spinEnergyKp->setValue(settings.value(_configNames[ConfigName::PowerGenKp]).toDouble());
    _ui->spinEnergyKi->setValue(settings.value(_configNames[ConfigName::PowerGenKi]).toDouble());
    _ui->spinEnergyKd->setValue(settings.value(_configNames[ConfigName::PowerGenKd]).toDouble());
    _ui->spinIntegralMaximumCorePump->setValue(settings.value(_configNames[ConfigName::PowerGenIntegralMax]).toDouble());
    _ui->checkEnableEnergyPID->setChecked(settings.value(_configNames[ConfigName::PowerGenEnabled]).toBool());

    _ui->spinRodMin->setValue(settings.value(_configNames[ConfigName::CoreTempRodMin]).toDouble());
    _ui->spinRodMax->setValue(settings.value(_configNames[ConfigName::CoreTempRodMax]).toDouble());
    _ui->spinRodKp->setValue(settings.value(_configNames[ConfigName::CoreTempKp]).toDouble());
    _ui->spinRodKi->setValue(settings.value(_configNames[ConfigName::CoreTempKi]).toDouble());
    _ui->spinRodKd->setValue(settings.value(_configNames[ConfigName::CoreTempKd]).toDouble());
    _ui->spinIntegralMaximumRods->setValue(settings.value(_configNames[ConfigName::CoreTempIntegralMax]).toDouble());
    _ui->spinTemperatureTarget->setValue(settings.value(_configNames[ConfigName::CoreTempTarget]).toDouble());
    _ui->checkEnableRodControl->setChecked(settings.value(_configNames[ConfigName::CoreTempEnabled]).toBool());
}

void MainWindow::StoreConfigs(){
    QSettings settings = QSettings(_settingsFilename, QSettings::Format::IniFormat);

    settings.setValue(_configNames[ConfigName::Ip], _ui->lineIP->text());
    settings.setValue(_configNames[ConfigName::SamplingPeriod], _ui->spinPeriod->value());

    settings.setValue(_configNames[ConfigName::SteamGenFlowMin], _ui->spinSteamFlowMin->value());
    settings.setValue(_configNames[ConfigName::SteamGenFlowMax], _ui->spinSteamFlowMax->value());
    settings.setValue(_configNames[ConfigName::SteamGenKp], _ui->spinSteamKp->value());
    settings.setValue(_configNames[ConfigName::SteamGenKi], _ui->spinSteamKi->value());
    settings.setValue(_configNames[ConfigName::SteamGenKd], _ui->spinSteamKd->value());
    settings.setValue(_configNames[ConfigName::SteamGenIntegralMax], _ui->spinIntegralMaximumGen->value());
    settings.setValue(_configNames[ConfigName::SteamGenVolumeTarget], _ui->spinGenVolumeTarget->value());
    settings.setValue(_configNames[ConfigName::SteamGenEnabled], _ui->checkEnableSteam->isChecked());

    settings.setValue(_configNames[ConfigName::SteamPressureValveMin], _ui->spinPressureValveMin->value());
    settings.setValue(_configNames[ConfigName::SteamPressureValveMax], _ui->spinPressureValveMax->value());
    settings.setValue(_configNames[ConfigName::SteamPressureKp], _ui->spinPressureKp->value());
    settings.setValue(_configNames[ConfigName::SteamPressureKi], _ui->spinPressureKi->value());
    settings.setValue(_configNames[ConfigName::SteamPressureKd], _ui->spinPressureKd->value());
    settings.setValue(_configNames[ConfigName::SteamPressureIntegralMax], _ui->spinPressureMaxIntegral->value());
    settings.setValue(_configNames[ConfigName::SteamPressureTarget], _ui->spinPressureTarget->value());
    settings.setValue(_configNames[ConfigName::SteamPressureEnabled], _ui->checkEnablePressureController->isChecked());

    settings.setValue(_configNames[ConfigName::PowerGenFlowMin], _ui->spinCorePumpMin->value());
    settings.setValue(_configNames[ConfigName::PowerGenFlowMax], _ui->spinCorePumpMax->value());
    settings.setValue(_configNames[ConfigName::PowerGenKp], _ui->spinEnergyKp->value());
    settings.setValue(_configNames[ConfigName::PowerGenKi], _ui->spinEnergyKi->value());
    settings.setValue(_configNames[ConfigName::PowerGenKd], _ui->spinEnergyKd->value());
    settings.setValue(_configNames[ConfigName::PowerGenIntegralMax], _ui->spinIntegralMaximumCorePump->value());
    settings.setValue(_configNames[ConfigName::PowerGenEnabled], _ui->checkEnableEnergyPID->isChecked());

    settings.setValue(_configNames[ConfigName::CoreTempRodMin], _ui->spinRodMin->value());
    settings.setValue(_configNames[ConfigName::CoreTempRodMax], _ui->spinRodMax->value());
    settings.setValue(_configNames[ConfigName::CoreTempKp], _ui->spinRodKp->value());
    settings.setValue(_configNames[ConfigName::CoreTempKi], _ui->spinRodKi->value());
    settings.setValue(_configNames[ConfigName::CoreTempKd], _ui->spinRodKd->value());
    settings.setValue(_configNames[ConfigName::CoreTempIntegralMax], _ui->spinIntegralMaximumRods->value());
    settings.setValue(_configNames[ConfigName::CoreTempTarget], _ui->spinTemperatureTarget->value());
    settings.setValue(_configNames[ConfigName::CoreTempEnabled], _ui->checkEnableRodControl->isChecked());
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
    double actuator, sensor, sensor2;

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

    if(_ui->checkEnableSteam->isChecked()){
        if(json["STEAM_GEN_0_STATUS"].toInt() ==2){
            actuator = json["COOLANT_SEC_CIRCULATION_PUMP_0_SPEED"].toDouble();
            sensor = json["COOLANT_SEC_0_LIQUID_VOLUME"].toDouble()/1000.0;
            _ui->lcd_L1_Steam->display(json["STEAM_GEN_0_OUTLET"].toDouble());
            _ui->lcd_L1_Coolant->display(actuator);
            _ui->lcd_L1_Coolant->display(sensor);
            OrderSteamFlow(0, actuator, sensor);
        }

        if(json["STEAM_GEN_1_STATUS"].toInt() ==2){
            actuator = json["COOLANT_SEC_CIRCULATION_PUMP_1_SPEED"].toDouble();
            sensor = json["COOLANT_SEC_1_LIQUID_VOLUME"].toDouble()/1000.0;
            _ui->lcd_L2_Steam->display(json["STEAM_GEN_1_OUTLET"].toDouble());
            _ui->lcd_L2_Coolant->display(actuator);
            _ui->lcd_L2_Coolant->display(sensor);
            OrderSteamFlow(1, actuator, sensor);
        }

        if(json["STEAM_GEN_2_STATUS"].toInt() ==2){
            actuator = json["COOLANT_SEC_CIRCULATION_PUMP_2_SPEED"].toDouble();
            sensor = json["COOLANT_SEC_2_LIQUID_VOLUME"].toDouble()/1000.0;
            _ui->lcd_L3_Steam->display(json["STEAM_GEN_2_OUTLET"].toDouble());
            _ui->lcd_L3_Valve->display(actuator);
            _ui->lcd_L3_Coolant->display(sensor);
            OrderSteamFlow(2, actuator, sensor);
        }
    }

    if(_ui->checkEnableEnergyPID->isChecked()){
        actuator = json["COOLANT_CORE_FLOW_ORDERED_SPEED"].toDouble();
        sensor = json["POWER_DEMAND_MW"].toDouble();
        sensor2 = json["GENERATOR_0_KW"].toDouble();
        sensor2 += json["GENERATOR_1_KW"].toDouble();
        sensor2 += json["GENERATOR_2_KW"].toDouble();
        sensor2 /= 1000.0;
        _ui->lcdGenPower->display(sensor2);
        _ui->lcdReqPower->display(sensor);
        _ui->lcdCorePump->display(actuator);
        OrderCorePump(actuator, sensor2, sensor);
    }

    reply->deleteLater();
}

void MainWindow::Timeout(){
    GetRequest("WEBSERVER_BATCH_GET");
}
