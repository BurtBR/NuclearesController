#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QVector>

#include "pidcontroller.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow{
    Q_OBJECT
public:
    enum class ConsoleMessageType{
        Info,
        Error,
        Warning
    };

    enum NuclearVariable{
        TIME,
        CORE_TEMP,
        CORE_STATE_CRITICALITY,
        COOLANT_SEC_0_VOLUME,
        COOLANT_SEC_1_VOLUME,
        COOLANT_SEC_2_VOLUME,
        STEAM_GEN_0_OUTLET,
        STEAM_GEN_1_OUTLET,
        STEAM_GEN_2_OUTLET,
        COOLANT_SEC_CIRCULATION_PUMP_0_SPEED,
        COOLANT_SEC_CIRCULATION_PUMP_1_SPEED,
        COOLANT_SEC_CIRCULATION_PUMP_2_SPEED,
        COOLANT_SEC_CIRCULATION_PUMP_0_ORDERED_SPEED,
        COOLANT_SEC_CIRCULATION_PUMP_1_ORDERED_SPEED,
        COOLANT_SEC_CIRCULATION_PUMP_2_ORDERED_SPEED,
        POWER_DEMAND_MW,
        COOLANT_CORE_FLOW_SPEED,
        COOLANT_CORE_FLOW_ORDERED_SPEED,
        GENERATOR_0_KW,
        GENERATOR_1_KW,
        GENERATOR_2_KW,
        GAME_SIM_SPEED,
        RODS_POS_ACTUAL,
        ROD_BANK_POS_0_ORDERED,
    };

private:
    Ui::MainWindow *_ui;
    QNetworkAccessManager *_netManager = nullptr;
    QTimer *_timer = nullptr;
    PIDController _energyPID = PIDController(0 ,0 ,0 , 0.5), _rodPID = PIDController(0, 0, 0, 0.1);
    QVector<PIDController> _steamPID = {PIDController(0,0,0,0.5), PIDController(0,0,0,0.5), PIDController(0,0,0,0.5)};
    QVector<int> _steamOutlet = {0,0,0}, _coolantVolume = {0,0,0}, _secCoolFlow = {0,0,0};
    double _requiredPower = 0, _generatedPower = 0;
    double _rodsActual = 0;
    double _coreTemp = 0;
    int _corePump = 0, _lastGameSpeed = 1;
    uint32_t _lastTime = 0;
    static const QMap<QString,NuclearVariable> _variablesNames;

    void ConsoleMessage(QString msg, ConsoleMessageType type = ConsoleMessageType::Info);
    void OrderRods();
    void OrderCorePump();
    void OrderSteamFlow(int number);
    void SetSteamButtonRed(int number);
    void SetSteamButtonGreen(int number);
    void SetPowerGenButtonRed();
    void SetPowerGenButtonGreen();
    void SetCoreTemperatureButtonRed();
    void SetCoreTemperatureButtonGreen();
    void StartController();
    void StopController();
    void ControlPlant();

    void PostRequest(QString variable, QString value);
    void GetRequest(QString variable);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void ReplyReceived(QNetworkReply *reply);
    void Timeout();
    void On_buttonStart_Clicked();
    void On_spinPeriod_editingFinished();
    void On_spinRodKp_editingFinished();
    void On_spinRodKi_editingFinished();
    void On_spinRodKd_editingFinished();
    void On_spinSteamKp_editingFinished();
    void On_spinSteamKi_editingFinished();
    void On_spinSteamKd_editingFinished();
    void On_spinEnergyKp_editingFinished();
    void On_spinEnergyKi_editingFinished();
    void On_spinEnergyKd_editingFinished();
    void On_spinIntegralMaximumGen_editingFinished();
    void On_spinIntegralMaximumCorePump_editingFinished();
    void On_spinIntegralMaximumRods_editingFinished();
    void On_checkEnableSteam_checkStatusChanged(Qt::CheckState state);
    void On_checkEnableEnergyPID_checkStatusChanged(Qt::CheckState state);
    void On_checkEnableRodControl_checkStatusChanged(Qt::CheckState state);

signals:
    void TimerStart(int msec);
    void TimerStop();
};
#endif // MAINWINDOW_H
