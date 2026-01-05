#include "CameraWorker.h"
#include <QVideoFrame>
#include <QImage>

CameraWorker::CameraWorker(QObject *parent)
    : QObject(parent), yolo_("yolov8n.onnx", true) {}

void CameraWorker::start(){ running_ = true; }
void CameraWorker::stop(){ running_ = false; }

void CameraWorker::processFrame(const QVideoFrame &frame) {
    if(!running_ || !frame.isValid()) return;

    QVideoFrame f(frame);
    f.map(QVideoFrame::ReadOnly);
    QImage image = f.toImage();
    f.unmap();

    cv::Mat mat(image.height(), image.width(), CV_8UC4, (void*)image.bits(), image.bytesPerLine());
    cv::Mat bgr;
    cv::cvtColor(mat, bgr, cv::COLOR_RGBA2BGR);

    auto results = yolo_.detect(bgr);

    // »­¿ò
    for(auto &d : results){
        cv::rectangle(bgr, d.box, cv::Scalar(0,255,0), 2);
    }

    cv::cvtColor(bgr, mat, cv::COLOR_BGR2RGBA);
    QImage out(image.bits(), image.width(), image.height(), image.bytesPerLine(), QImage::Format_RGBA8888);
    emit frameReady(out.copy());
}
