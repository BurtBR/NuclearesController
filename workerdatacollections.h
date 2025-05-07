#ifndef WORKERDATACOLLECTIONS_H
#define WORKERDATACOLLECTIONS_H

#include <QObject>
#include <QTimer>
#include "mainwindow.h"

class WorkerDataCollections : public QObject{
    Q_OBJECT
private:
    QTimer *_timer = nullptr;

public:
    explicit WorkerDataCollections(QObject *parent = nullptr);
    ~WorkerDataCollections();

private slots:
    void Timeout();

public slots:
    void StartCollection(int msec, QString filename);
    void StopCollection();

signals:
    void Message(QString text, MainWindow::ConsoleMessageType type = MainWindow::ConsoleMessageType::Info);
    void Error();
    void CollectionFinished();
};

#endif // WORKERDATACOLLECTIONS_H
