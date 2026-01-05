#include "MainWindow.h"
#include <QVBoxLayout>
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    videoLabel_ = new QLabel(this);
    videoLabel_->setMinimumSize(640,480);

    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->addWidget(videoLabel_);
    setCentralWidget(central);

    camera_ = new QCamera(this);
    videoSink_ = new QVideoSink(this);

    camera_->setCameraFormat(QCameraFormat());
    camera_->start();

    worker_ = new CameraWorker;
    QThread *thread = new QThread;
    worker_->moveToThread(thread);
    thread->start();

    connect(videoSink_, &QVideoSink::videoFrameChanged,
            worker_, &CameraWorker::processFrame);
    connect(worker_, &CameraWorker::frameReady,
            videoLabel_, [this](const QImage &img){
        videoLabel_->setPixmap(QPixmap::fromImage(img));
    });

    QMediaCaptureSession *session = new QMediaCaptureSession(this);
    session->setCamera(camera_);
    session->setVideoSink(videoSink_);
}

MainWindow::~MainWindow(){}
