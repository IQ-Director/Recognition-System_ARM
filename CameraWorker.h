#pragma once
#include <QObject>
#include <QVideoFrame>
#include <QMutex>
#include <QWaitCondition>
#include <opencv2/opencv.hpp>
#include "yolov8_onnx.h"

class CameraWorker : public QObject {
    Q_OBJECT
public:
    CameraWorker(QObject *parent=nullptr);
    void start();
    void stop();

signals:
    void frameReady(const QImage &frame);

public slots:
    void processFrame(const QVideoFrame &frame);

private:
    YOLOv8 yolo_;
    bool running_ = true;
};
