#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoWidget>
#include <QMediaDevices>
#include <zbar.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonCamera1_clicked();

    void on_pushButtonCamera2_clicked();

    void on_pushButtonTestQRCodDetection_clicked();

private:
    Ui::MainWindow *ui;
    QCamera *camera1;
    QCamera *camera2;
    QMediaCaptureSession *captureSession1;
    QMediaCaptureSession *captureSession2;
    zbar::ImageScanner scanner;
    void detectQRCodeFromCamera(QCamera *camera);
    void initializeCameras();
};

#endif // MAINWINDOW_H
