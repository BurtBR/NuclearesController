#ifndef WORKERDATACOLLECTIONS_H
#define WORKERDATACOLLECTIONS_H

#include <QObject>
#include <QTimer>
#include <QFile>
#include <QMap>
#include <QByteArray>
#include <QNetworkAccessManager>
#include "mainwindow.h"

class WorkerDataCollections : public QObject{
    Q_OBJECT
private:
    QNetworkAccessManager *_netManager = nullptr;
    QTimer *_timer = nullptr;
    QFile *_file = nullptr;
    QMap<MainWindow::NuclearVariable, QByteArray> _collectedVariables;
    QString _ip;
    int _qtyReceived = 0;

    void GetVariable(MainWindow::NuclearVariable variable);
    bool WriteData();

public:
    explicit WorkerDataCollections(QObject *parent = nullptr);
    ~WorkerDataCollections();

    void AddVariable(MainWindow::NuclearVariable variable);

private slots:
    void Timeout();
    void ServerReply(QNetworkReply *reply);

public slots:
    void StartCollection(int msec, QString filename, QString ip);
    void StopCollection();

signals:
    void TimerStart(int msec);
    void TimerStop();
    void Message(QString text, MainWindow::ConsoleMessageType type = MainWindow::ConsoleMessageType::Info);
    void CollectionFinished();
};

#endif // WORKERDATACOLLECTIONS_H
