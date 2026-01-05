#pragma once
#include <QMainWindow>
#include <QVideoSink>
#include <QCamera>
#include <QLabel>
#include "CameraWorker.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent=nullptr);
    ~MainWindow();

private:
    QCamera *camera_;
    QVideoSink *videoSink_;
    CameraWorker *worker_;
    QLabel *videoLabel_;
};
