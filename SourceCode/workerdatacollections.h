#ifndef WORKERDATACOLLECTIONS_H
#define WORKERDATACOLLECTIONS_H

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
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
    QElapsedTimer *_elapsedTimer = nullptr;
    QFile *_file = nullptr;
    QMap<QString, QString> _collectedVariables;
    QString _ip;

    void GetVariable(QString variable);
    bool WriteData();

public:
    explicit WorkerDataCollections(QObject *parent = nullptr);
    ~WorkerDataCollections();

    void AddVariable(QString variable);

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
