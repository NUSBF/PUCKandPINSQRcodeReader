#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMediaDevices>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoWidget>
#include <QDebug>
#include <QThread>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <QElapsedTimer>
#include <QTimer>
#include <QImage>
#include <QImageCapture>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QGraphicsScene>
#include <QVideoSink>
#include <QDir>
#include <zbar.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , camera1(nullptr)
    , camera2(nullptr)
    , captureSession1(new QMediaCaptureSession(this))
    , captureSession2(new QMediaCaptureSession(this))
{
    ui->setupUi(this);
    initializeCameras();  // Call to initializeCameras without parameters
}
void MainWindow::initializeCameras()
{
    // Access video widgets directly from UI
    QVideoWidget *videoWidget1 = ui->widgetCamera1;
    QVideoWidget *videoWidget2 = ui->widgetCamera2;

    QList<QCameraDevice> cameras = QMediaDevices::videoInputs();

    if (cameras.size() < 2)
    {
        qDebug() << "Less than two cameras found.";
        return;
    }

    // Debugging: Check camera descriptions
    qDebug() << "Camera 1: " << cameras[0].description();
    qDebug() << "Camera 2: " << cameras[1].description();

    // Use QCamera to handle the video feed
    camera1 = new QCamera(cameras[1], this);
    camera2 = new QCamera(cameras[0], this);

    // Set up the capture sessions for each camera
    captureSession1->setCamera(camera1);
    captureSession1->setVideoOutput(videoWidget1);

    captureSession2->setCamera(camera2);
    captureSession2->setVideoOutput(videoWidget2);

    // Start the first camera
    //camera1->start();
    qDebug() << "Camera 1 started";

    // Delay to ensure the first camera initializes before starting the second one
    QThread::sleep(1);  // Small delay to give time for the first camera to initialize

    // Start the second camera
    //camera2->start();
    qDebug() << "Camera 2 started";
}

MainWindow::~MainWindow()
{
    delete ui;
    if (camera1) camera1->stop();
    if (camera2) camera2->stop();
}

void MainWindow::on_pushButtonCamera2_clicked()
{
    camera2->start();
    detectQRCodeFromCamera(camera2);

}

void MainWindow::on_pushButtonCamera1_clicked()
{
    // Start the camera feed
    camera1->start();
    detectQRCodeFromCamera(camera1);

}

void MainWindow::detectQRCodeFromCamera(QCamera *camera)
{
    if (!ui->graphicsViewCamera->scene()) {
        ui->graphicsViewCamera->setScene(new QGraphicsScene(this));
    }

    QMediaCaptureSession *currentSession = (camera == camera1) ? captureSession1 : captureSession2;
    QImageCapture *imageCapture = new QImageCapture(this);
    currentSession->setImageCapture(imageCapture);

    connect(imageCapture, &QImageCapture::imageCaptured, this,
            [this](int id, const QImage &image) {
                QPixmap pixmap = QPixmap::fromImage(image);
                ui->graphicsViewCamera->scene()->clear();
                ui->graphicsViewCamera->scene()->addPixmap(pixmap);
                QImage gray = image.convertToFormat(QImage::Format_Grayscale8);
                QString filename = QString("/home/linuxhomes/namlb/Downloads/QR.png");
                gray.save(filename);
            });

    imageCapture->capture();

}

void MainWindow::on_pushButtonTestQRCodDetection_clicked()
{
    // First test the known working QR image
    //QImage testImage("/home/linuxhomes/namlb/Downloads/testqrcode.png");  // the one that worked
    QImage testImage("/home/linuxhomes/namlb/Downloads/smallQR.png");
    QImage testGray = testImage.convertToFormat(QImage::Format_Grayscale8);
    qDebug() << "\nTesting known good image:";
    qDebug() << "Test image - Size:" << testGray.width() << "x" << testGray.height()
             << "Format:" << testGray.format()
             << "BytesPerLine:" << testGray.bytesPerLine();

    zbar::Image test_zbar(testGray.width(), testGray.height(), "Y800",
                          testGray.constBits(), testGray.sizeInBytes());
    int test_result = scanner.scan(test_zbar);
    qDebug() << "Known good image scan result:" << test_result;

    // Now test the camera image
    QGraphicsScene* scene = ui->graphicsViewCamera->scene();
    if (!scene) {
        qDebug() << "No scene available";
        return;
    }

    QList<QGraphicsItem*> items = scene->items();
    if (items.isEmpty()) {
        qDebug() << "No items in scene";
        return;
    }

    QGraphicsPixmapItem* pixmapItem = qgraphicsitem_cast<QGraphicsPixmapItem*>(items.first());
    if (!pixmapItem) {
        qDebug() << "No pixmap item found";
        return;
    }

    QImage gray = pixmapItem->pixmap().toImage().convertToFormat(QImage::Format_Grayscale8);
    qDebug() << "\nTesting camera image:";
    qDebug() << "Camera image - Size:" << gray.width() << "x" << gray.height()
             << "Format:" << gray.format()
             << "BytesPerLine:" << gray.bytesPerLine();

    zbar::Image zbar_image(gray.width(), gray.height(), "Y800",
                           gray.constBits(), gray.sizeInBytes());

    int result = scanner.scan(zbar_image);
    qDebug() << "Camera image scan result:" << result;
}
