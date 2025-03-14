#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoWidget>
#include <QMediaDevices>
#include <QElapsedTimer>
#include <QThread>

// Define the CircleInfo structure independently
struct CircleInfo
{
    int id = 0;
    QPointF center;
    int radius;
    QString dmCode;
    bool hasDM = false;
    double distanceToCentroid = 0.0;
    double angle = 0.0;
};

// Forward declare the ImageProcessor
class ImageProcessor;

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

    // Handler for image processor results
    void handleDataMatrixFound(int circleId, const QString &code, const QImage &processedImage);
    void updateDebugView(int circleId, const QImage &image);

signals:
    // Signal to send images to processor
    void newImageForProcessing(const QImage &image, const QVector<CircleInfo> &circles,
                               const QSet<int> &processedCircles);

private:
    Ui::MainWindow *ui;
    QCamera *camera1;
    QCamera *camera2;
    QMediaCaptureSession *captureSession1;
    QMediaCaptureSession *captureSession2;
    QElapsedTimer dmDetectionTimer;
    bool setupCameraCapture(QCamera *camera, QMediaCaptureSession *&session, QImageCapture *&imageCapture);
    bool detectCircles(QImageCapture *imageCapture, QVector<CircleInfo> &circles);
    // Add these to the private section of your MainWindow class
    bool identifyCirclePositions(QVector<CircleInfo> &circles, QPointF &centroid);
    void displayIdentifiedCircles(QVector<CircleInfo> &circles, QPointF centroid);
    bool detectDataMatrices(QImageCapture *imageCapture, QVector<CircleInfo> &circles);
    void assignCircleIDs(QVector<CircleInfo> &innerCircles, QVector<CircleInfo> &outerCircles, int innerRefIndex, int outerRefIndex);
    QImage processCircleRegion(const QImage &sourceImage, const CircleInfo &circleInfo);
    void detectQRCodeFromCameraPuck(QCamera *camera);
    void detectQRCodeFromCameraVideo(QCamera *camera, int targetDMCount);
    void detectQRCodeFromCameraPin(QCamera *camera);
    void initializeCameras();
    QImage enhanceImage(const QImage &gray);
    void updateCircleDebugView(const CircleInfo &circleInfo, const QImage &processedImage, bool found = false, const QString &code = QString());
    QThread workerThread;
    ImageProcessor *imageProcessor;
    QVector<CircleInfo> currentCircles;
};

// Define ImageProcessor as a separate class
class ImageProcessor : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcessor(QObject *parent = nullptr);
    ~ImageProcessor();

public slots:
    void processImage(const QImage &image, const QVector<CircleInfo> &circles,
                      const QSet<int> &processedCircles);

signals:
    void dataMatrixFound(int circleId, const QString &code, const QImage &processedImage);
    void processingComplete(int processedCount);
    void debugImage(int circleId, const QImage &image);

private:
    QImage processCircleRegion(const QImage &sourceImage, const CircleInfo &circleInfo);
    bool detectDataMatrixInCircle(const QImage &processedImage, CircleInfo &circleInfo);
};

#endif // MAINWINDOW_H
