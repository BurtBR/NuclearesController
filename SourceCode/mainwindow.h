#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QVector>
#include <QThread>
#include <QSettings>

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

private:
    enum class ConfigName{
        Ip,
        SamplingPeriod,
        SteamGenFlowMin,
        SteamGenFlowMax,
        SteamGenKp,
        SteamGenKi,
        SteamGenKd,
        SteamGenIntegralMax,
        SteamGenVolumeTarget,
        SteamGenEnabled,
        SteamPressureValveMin,
        SteamPressureValveMax,
        SteamPressureKp,
        SteamPressureKi,
        SteamPressureKd,
        SteamPressureIntegralMax,
        SteamPressureTarget,
        SteamPressureEnabled,
        PowerGenFlowMin,
        PowerGenFlowMax,
        PowerGenKp,
        PowerGenKi,
        PowerGenKd,
        PowerGenIntegralMax,
        PowerGenEnabled,
        CoreTempRodMin,
        CoreTempRodMax,
        CoreTempKp,
        CoreTempKi,
        CoreTempKd,
        CoreTempIntegralMax,
        CoreTempTarget,
        CoreTempEnabled
    };
    static const QString _settingsFilename;
    Ui::MainWindow *_ui;
    QNetworkAccessManager *_netManager = nullptr;
    QTimer *_timer = nullptr;
    QThread *_threadDataCollection = nullptr;
    static const QMap<ConfigName,QString> _configNames;

    PIDController _energyPID = PIDController(0 ,0 ,0 , 0.5), _rodPID = PIDController(0, 0, 0, 0.1);

    QVector<PIDController> _steamPID = {PIDController(0,0,0,0.5), PIDController(0,0,0,0.5), PIDController(0,0,0,0.5)};

    QVector<PIDController> _pressurePID = {PIDController(), PIDController(), PIDController()};

    void DeleteThread(QThread **thread);
    void OrderRods(double rodPosition, double coreTemp);
    void OrderCorePump(double pump, double generated, double required);
    void OrderSteamFlow(int number, double valve, double volume);
    void OrderPressure(int number, double valve, double pressure);
    void SetSteamButtonRed(int number);
    void SetSteamButtonGreen(int number);
    void SetPressureButtonRed(int number);
    void SetPressureButtonGreen(int number);
    void SetPowerGenButtonRed();
    void SetPowerGenButtonGreen();
    void SetCoreTemperatureButtonRed();
    void SetCoreTemperatureButtonGreen();
    void StartController();
    void StopController();
    void GetConfigs();
    void StoreConfigs();

    void PostRequest(QString variable, QString value);
    void GetRequest(QString variable);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void ConsoleMessage(QString msg, ConsoleMessageType type = ConsoleMessageType::Info);
    void ReplyReceived(QNetworkReply *reply);
    void Timeout();
    void CollectionFinished();
    void On_buttonStart_Clicked();
    void On_buttonStartDataCollection_Clicked();
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
    void On_spinPressureKp_editingFinihed();
    void On_spinPressureKi_editingFinihed();
    void On_spinPressureKd_editingFinihed();
    void On_spinIntegralMaximumGen_editingFinished();
    void On_spinIntegralMaximumCorePump_editingFinished();
    void On_spinIntegralMaximumRods_editingFinished();
    void On_spinPressureMaxIntegral_editingFinished();
    void On_checkEnableSteam_checkStatusChanged(Qt::CheckState state);
    void On_checkEnableEnergyPID_checkStatusChanged(Qt::CheckState state);
    void On_checkEnableRodControl_checkStatusChanged(Qt::CheckState state);
    void On_checkEnablePressureController_checkedStatusChanged(Qt::CheckState state);

signals:
    void TimerStart(int msec);
    void TimerStop();
    void StartCollection(int msec, QString filename, QString ip);
    void StopCollection();
};
#endif // MAINWINDOW_H
