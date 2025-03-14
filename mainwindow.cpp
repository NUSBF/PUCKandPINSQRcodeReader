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
#include <QGraphicsBlurEffect>
#include <QVideoSink>
#include <QDir>
#include <ZXing/ReadBarcode.h>
#include <ZXing/BarcodeFormat.h>
#include <ZXing/DecodeHints.h>
#include <ZXing/Result.h>
#include <QMessageBox>


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
    // Create image processor and move to worker thread
    imageProcessor = new ImageProcessor();
    imageProcessor->moveToThread(&workerThread);

    // Connect signals/slots
    connect(&workerThread, &QThread::finished, imageProcessor, &QObject::deleteLater);
    connect(this, &MainWindow::newImageForProcessing, imageProcessor, &ImageProcessor::processImage);
    connect(imageProcessor, &ImageProcessor::dataMatrixFound, this, &MainWindow::handleDataMatrixFound);
    connect(imageProcessor, &ImageProcessor::debugImage, this, &MainWindow::updateDebugView);

    // Start worker thread
    workerThread.start();
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
    workerThread.quit();
    workerThread.wait();
    delete ui;
    if (camera1) camera1->stop();
    if (camera2) camera2->stop();
}

void MainWindow::on_pushButtonCamera2_clicked()
{
    camera2->start();

    // Get the device info for camera2
    QCameraDevice cameraDevice = camera2->cameraDevice();

    qDebug() << "Camera2: " << cameraDevice.description();
    qDebug() << "- ID: " << cameraDevice.id();
    qDebug() << "- Position: " << cameraDevice.position();

    // Check the available video formats
    const QList<QCameraFormat> formats = cameraDevice.videoFormats();
    if (!formats.isEmpty()) {
        qDebug() << "- Supports" << formats.size() << "video formats";
        qDebug() << "- Max resolution:" << formats.first().resolution().width()
                 << "x" << formats.first().resolution().height();
    }

    // Check minimum/maximum frame rate
    if (!formats.isEmpty()) {
        qDebug() << "- Min framerate:" << formats.first().minFrameRate();
        qDebug() << "- Max framerate:" << formats.first().maxFrameRate();
    }

    detectQRCodeFromCameraPin(camera2);
}

void MainWindow::on_pushButtonCamera1_clicked()
{
    // Start the camera feed
    camera1->start();
    detectQRCodeFromCameraPuck(camera1);

}

void MainWindow::on_pushButtonTestQRCodDetection_clicked()
{
    // First test the known working QR image
    //QImage testImage("/home/linuxhomes/namlb/Documents/QT/USBpuckcamera/resources/PUCK_DM.png");
    //QImage testImage("/home/linuxhomes/namlb/Documents/QT/USBpuckcamera/resources/smallDM.png");
    QImage testImage("/home/linuxhomes/namlb/Documents/QT/USBpuckcamera/resources/DM.png");
    if (testImage.isNull()) {
        qDebug() << "Error: Failed to load test image.";
        return;
    }
    QImage testGray = testImage.convertToFormat(QImage::Format_Grayscale8);

    qDebug() << "\nTesting known good image:";
    qDebug() << "Test image - Size:" << testGray.width() << "x" << testGray.height()
             << " Format:" << testGray.format()
             << " BytesPerLine:" << testGray.bytesPerLine();

    ZXing::DecodeHints hints;
    hints.setTryHarder(true);
    hints.setTryRotate(true);
    hints.setFormats(ZXing::BarcodeFormat::DataMatrix);

    ZXing::ImageView testView(testGray.constBits(), testGray.width(), testGray.height(), ZXing::ImageFormat::Lum);
    QGraphicsScene* scene = new QGraphicsScene(this);
    scene->addPixmap(QPixmap::fromImage(testGray));
    ui->graphicsViewCamera->setScene(scene);

    ZXing::Result testResult = ZXing::ReadBarcode(testView, hints);

    if (testResult.isValid())
    {
        qDebug() << "Known good image scan result:" << QString::fromStdString(testResult.text());
    }
    else
    {
        qDebug() << "DM Code not detected in the test image.";
    }

}

void MainWindow::detectQRCodeFromCameraPuck(QCamera *camera)
{
    if (!ui->graphicsViewCamera->scene()) {
        ui->graphicsViewCamera->setScene(new QGraphicsScene(this));
    }

    QMediaCaptureSession *currentSession = (camera == camera1) ? captureSession1 : captureSession2;
    QImageCapture *imageCapture = new QImageCapture(this);
    currentSession->setImageCapture(imageCapture);

    // Create a shared persistent storage for detected results
    struct SharedState {
        QSet<QString> detectedDMs;
        QMap<QString, QPointF> dmPositions; // Store positions of detected DM codes
    };

    SharedState *sharedState = new SharedState();
    this->dmDetectionTimer.start();

    QTimer *captureTimer = new QTimer(this);

    connect(imageCapture, &QImageCapture::imageCaptured, this,
            [this, sharedState, captureTimer, imageCapture](int id, const QImage &originalImage) {
                // Create a copy of the original image for processing
                QImage image = originalImage.copy();

                // Convert to grayscale first
                QImage gray = image.convertToFormat(QImage::Format_Grayscale8);

                // Apply unsharp mask for sharpening
                QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect();
                blurEffect->setBlurRadius(10.0);

                QGraphicsScene scene;
                QGraphicsPixmapItem item;
                item.setPixmap(QPixmap::fromImage(gray));
                item.setGraphicsEffect(blurEffect);
                scene.addItem(&item);

                QImage blurredResult(gray.size(), QImage::Format_Grayscale8);
                blurredResult.fill(Qt::transparent);
                QPainter painter(&blurredResult);
                scene.render(&painter);
                painter.end();

                // Calculate unsharp mask
                double amount = 1.0;
                QImage enhanced(gray.size(), QImage::Format_Grayscale8);
                for (int y = 0; y < gray.height(); y++) {
                    for (int x = 0; x < gray.width(); x++) {
                        int originalVal = qGray(gray.pixel(x, y));
                        int blurredVal = qGray(blurredResult.pixel(x, y));

                        int sharpened = qBound(0,
                                               (int)(originalVal + amount * (originalVal - blurredVal)),
                                               255);

                        enhanced.setPixel(x, y, qRgb(sharpened, sharpened, sharpened));
                    }
                }

                // Convert enhanced QImage to OpenCV Mat
                cv::Mat enhancedMat(enhanced.height(), enhanced.width(), CV_8UC1,
                                    const_cast<uchar*>(enhanced.constBits()), enhanced.bytesPerLine());

                // Detect circles using HoughCircles with tuned parameters
                std::vector<cv::Vec3f> circles;
                cv::HoughCircles(enhancedMat, circles, cv::HOUGH_GRADIENT, 1,
                                 enhancedMat.rows/20,
                                 100, 40,
                                 40, 50);

                // Create a colored image for visualization
                QImage displayImage = enhanced.convertToFormat(QImage::Format_RGB32);
                QPainter displayPainter(&displayImage);
                displayPainter.setRenderHint(QPainter::Antialiasing);

                // Set to track codes found in the current frame
                QSet<QString> currentFrameDMCodes;
                QMap<int, QString> circleToCodeMap; // Map circle index to DM code found

                // Process each circle for DM codes
                for (size_t i = 0; i < circles.size(); i++) {
                    const auto& circle = circles[i];
                    int x = cvRound(circle[0]);
                    int y = cvRound(circle[1]);
                    int radius = cvRound(circle[2]);

                    // Extract region around this circle for DataMatrix detection
                    int startX = qMax(0, x - radius);
                    int startY = qMax(0, y - radius);
                    int endX = qMin(enhanced.width() - 1, x + radius);
                    int endY = qMin(enhanced.height() - 1, y + radius);
                    int width = endX - startX;
                    int height = endY - startY;

                    if (width <= 0 || height <= 0) continue;

                    QImage circleRegion = enhanced.copy(startX, startY, width, height);

                    // Process this region for DataMatrix
                    ZXing::DecodeHints hints;
                    hints.setTryHarder(true);
                    hints.setTryRotate(true);
                    hints.setFormats(ZXing::BarcodeFormat::DataMatrix);

                    ZXing::ImageView regionView(circleRegion.constBits(),
                                                circleRegion.width(),
                                                circleRegion.height(),
                                                ZXing::ImageFormat::Lum);

                    std::vector<ZXing::Result> results = ZXing::ReadBarcodes(regionView, hints);

                    bool dmFoundInThisCircle = false;
                    for (const auto& result : results) {
                        if (result.isValid()) {
                            QString code = QString::fromStdString(result.text());
                            currentFrameDMCodes.insert(code);

                            // Store which DM code was found in which circle
                            circleToCodeMap[i] = code;

                            // Add to overall collection if it's new
                            if (!sharedState->detectedDMs.contains(code)) {
                                sharedState->detectedDMs.insert(code);
                                // Store the position where this code was found
                                sharedState->dmPositions[code] = QPointF(x, y);
                                qDebug() << "New DM code found:" << code << "in circle at (" << x << "," << y << ")";
                            }

                            dmFoundInThisCircle = true;

                            // Draw rectangle around code
                            auto points = result.position();
                            displayPainter.setPen(QPen(Qt::green, 2));
                            displayPainter.drawRect(
                                startX + points.topLeft().x,
                                startY + points.topLeft().y,
                                points.bottomRight().x - points.topLeft().x,
                                points.bottomRight().y - points.topLeft().y);

                            break; // Only use the first code found in this circle
                        }
                    }

                    // First, check if any previously found DM code matches this circle's position
                    bool matchesExistingDetection = false;
                    QString existingCode;
                    const int proximityThreshold = 30; // Threshold for position matching (in pixels)

                    for (auto it = sharedState->dmPositions.begin(); it != sharedState->dmPositions.end(); ++it) {
                        QPointF storedPos = it.value();
                        double dist = qSqrt(qPow(storedPos.x() - x, 2) + qPow(storedPos.y() - y, 2));
                        if (dist <= proximityThreshold) {
                            matchesExistingDetection = true;
                            existingCode = it.key();
                            // Update the position with current circle's position
                            it.value() = QPointF(x, y);
                            break;
                        }
                    }

                    // Now color the circle based on detection status
                    if (dmFoundInThisCircle || matchesExistingDetection) {
                        // Draw green circle if DM found in this or previous frames
                        displayPainter.setPen(QPen(Qt::green, 2));

                        // Display the code text
                        displayPainter.setPen(Qt::green);
                        QString codeToShow = dmFoundInThisCircle ?
                                                 circleToCodeMap[i] :
                                                 existingCode;
                        displayPainter.drawText(
                            QPoint(x - 20, y + radius + 20),
                            codeToShow);
                    } else {
                        // Draw red circle if no DM found
                        displayPainter.setPen(QPen(Qt::red, 2));
                    }

                    displayPainter.drawEllipse(QPoint(x, y), radius, radius);
                }

                // Display all previously found DM codes that weren't found in this frame
                for (const QString& code : sharedState->detectedDMs) {
                    if (!currentFrameDMCodes.contains(code)) {
                        QPointF pos = sharedState->dmPositions[code];
                        displayPainter.setPen(QPen(Qt::yellow, 2)); // Use yellow for previously found codes
                        displayPainter.drawText(
                            QPoint(pos.x() - 20, pos.y() + 20),
                            "Previously: " + code);
                    }
                }

                displayPainter.end();

                // Display the resulting image
                QPixmap resultPixmap = QPixmap::fromImage(displayImage);
                ui->graphicsViewCamera->scene()->clear();
                ui->graphicsViewCamera->scene()->addPixmap(resultPixmap);

                // Add text showing counts
                QGraphicsTextItem* text = ui->graphicsViewCamera->scene()->addText(
                    QString("Found %1 circles | Total DM codes: %2 | Time: %3 sec")
                        .arg(circles.size())
                        .arg(sharedState->detectedDMs.size())
                        .arg(this->dmDetectionTimer.elapsed() / 1000.0, 0, 'f', 1));
                text->setDefaultTextColor(Qt::white);
                text->setPos(10, 10);

                // Check if we should continue
                if (this->dmDetectionTimer.elapsed() < 10000 && sharedState->detectedDMs.size() < 16) {
                    // Continue detection
                } else {
                    qDebug() << "Detection complete. Found" << sharedState->detectedDMs.size() << "unique DM codes in"
                             << this->dmDetectionTimer.elapsed() / 1000.0 << "seconds.";

                    captureTimer->stop();
                    captureTimer->deleteLater();
                    imageCapture->deleteLater();
                    delete sharedState;
                }
            });

    connect(captureTimer, &QTimer::timeout, this, [imageCapture]() {
        imageCapture->capture();
    });

    captureTimer->start(200);
    imageCapture->capture();
}

void MainWindow::handleDataMatrixFound(int circleId, const QString &code, const QImage &processedImage)
{
    // Find the circle with this ID and update it
    for (CircleInfo &circle : currentCircles) {
        if (circle.id == circleId) {
            circle.hasDM = true;
            circle.dmCode = code;

            // Update the debug view
            updateCircleDebugView(circle, processedImage, true, code);
            break;
        }
    }
}

void MainWindow::updateDebugView(int circleId, const QImage &image)
{
    // Find the circle with this ID
    for (const CircleInfo &circle : currentCircles) {
        if (circle.id == circleId) {
            // Update the debug view (searching state)
            updateCircleDebugView(circle, image);
            break;
        }
    }
}

void MainWindow::detectQRCodeFromCameraPin(QCamera *camera)
{
    // Create a scene if needed
    if (!ui->graphicsViewCamera->scene())
    {
        ui->graphicsViewCamera->setScene(new QGraphicsScene(this));
    }

    // Setup camera capture
    QMediaCaptureSession *captureSession = nullptr;
    QImageCapture *imageCapture = nullptr;
    if (!setupCameraCapture(camera, captureSession, imageCapture))
    {
        qDebug() << "Failed to setup camera capture";
        return;
    }

    // Detect circles
    QVector<CircleInfo> circles;
    if (!detectCircles(imageCapture, circles))
    {
        qDebug() << "Failed to detect circles";
        imageCapture->deleteLater();
        return;
    }

    // Identify circle positions
    QPointF centroid;
    if (!identifyCirclePositions(circles, centroid))
    {
        qDebug() << "Failed to identify circle positions";
        imageCapture->deleteLater();
        return;
    }

    // Display identified circles
    displayIdentifiedCircles(circles, centroid);

    // Detect DataMatrix codes in circles
    if (!detectDataMatrices(imageCapture, circles))
    {
        qDebug() << "Failed to detect all DataMatrix codes";
    }

    // Clean up
    imageCapture->deleteLater();
}

bool MainWindow::setupCameraCapture(QCamera *camera, QMediaCaptureSession *&session, QImageCapture *&imageCapture)
{
    // Use existing capture session
    session = (camera == camera1) ? captureSession1 : captureSession2;
    imageCapture = new QImageCapture(this);
    session->setImageCapture(imageCapture);

    return true;
}

bool MainWindow::detectCircles(QImageCapture *imageCapture, QVector<CircleInfo> &circles)
{
    QElapsedTimer circleTimer;
    circleTimer.start();
    bool circlesIdentified = false;
    QGraphicsScene *scene = ui->graphicsViewCamera->scene();
    QVector<QPointF> foundCircleCenters;

    while (!circlesIdentified && circleTimer.elapsed() < 2000)
    {
        // Capture image
        QImage capturedImage;
        QEventLoop loop;
        auto connection = connect(imageCapture, &QImageCapture::imageCaptured, this,
                                  [&capturedImage, &loop](int id, const QImage &img)
                                  {
                                      qDebug() << "Image captured:" << id;
                                      capturedImage = img;
                                      loop.quit();
                                  });
        imageCapture->capture();
        loop.exec();
        disconnect(connection);

        if (capturedImage.isNull())
        {
            continue;
        }

        // Convert to grayscale and enhance image
        QImage gray = capturedImage.convertToFormat(QImage::Format_Grayscale8);
        QImage enhanced = enhanceImage(gray);

        // Convert to OpenCV Mat for circle detection
        cv::Mat enhancedMat(enhanced.height(), enhanced.width(), CV_8UC1,
                            const_cast<uchar*>(enhanced.constBits()), enhanced.bytesPerLine());

        // Detect circles
        std::vector<cv::Vec3f> detectedCircles;
        cv::HoughCircles(enhancedMat, detectedCircles, cv::HOUGH_GRADIENT, 1,
                         enhancedMat.rows/20, 100, 40, 40, 50);

        // Process detected circles
        QImage displayImage = capturedImage.copy();
        QPainter displayPainter(&displayImage);
        displayPainter.setRenderHint(QPainter::Antialiasing);

        // Filter and add circles
        for (const auto& circle : detectedCircles)
        {
            QPointF center(cvRound(circle[0]), cvRound(circle[1]));
            bool tooClose = false;

            // Check if this circle is too close to previously found ones
            for (const auto& existingCenter : foundCircleCenters)
            {
                double dx = center.x() - existingCenter.x();
                double dy = center.y() - existingCenter.y();
                double distance = qSqrt(dx*dx + dy*dy);

                if (distance < circle[2] * 0.5)
                {
                    tooClose = true;
                    break;
                }
            }

            if (!tooClose)
            {
                foundCircleCenters.append(center);

                CircleInfo info;
                info.center = center;
                info.radius = cvRound(circle[2]);
                circles.append(info);
            }
        }

        // Draw circles
        for (const auto& circleInfo : circles)
        {
            displayPainter.setPen(QPen(Qt::red, 2));
            displayPainter.drawEllipse(circleInfo.center, circleInfo.radius, circleInfo.radius);
        }
        displayPainter.end();

        // Display the image
        scene->clear();
        scene->addPixmap(QPixmap::fromImage(displayImage));

        // Check if we have enough circles
        if (circles.size() >= 16)
        {
            circlesIdentified = true;
        }

        qDebug() << "Found circles:" << circles.size() << "Elapsed time:" << circleTimer.elapsed() << "ms";
        QApplication::processEvents();
    }

    return circlesIdentified;
}

QImage MainWindow::enhanceImage(const QImage &gray)
{
    // Create a separate scene for the blur effect to avoid interfering with main scene
    QGraphicsScene tempScene;

    // Use QGraphicsBlurEffect with proper cleanup
    QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect();
    blurEffect->setBlurRadius(10.0);

    // Create a graphics item with the image
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(QPixmap::fromImage(gray));
    item->setGraphicsEffect(blurEffect);
    tempScene.addItem(item); // Scene takes ownership of the item

    // Render the blurred result
    QImage blurredResult(gray.size(), QImage::Format_Grayscale8);
    blurredResult.fill(Qt::transparent);
    QPainter painter(&blurredResult);
    tempScene.render(&painter);
    painter.end();

    // Calculate unsharp mask (no memory issues here)
    double amount = 1.0;
    QImage enhanced(gray.size(), QImage::Format_Grayscale8);
    for (int y = 0; y < gray.height(); y++)
    {
        for (int x = 0; x < gray.width(); x++)
        {
            int originalVal = qGray(gray.pixel(x, y));
            int blurredVal = qGray(blurredResult.pixel(x, y));
            int sharpened = qBound(0, (int)(originalVal + amount * (originalVal - blurredVal)), 255);
            enhanced.setPixel(x, y, qRgb(sharpened, sharpened, sharpened));
        }
    }

    // The scene will clean up the item and effect when it's destroyed
    return enhanced;
}

bool MainWindow::identifyCirclePositions(QVector<CircleInfo> &circles, QPointF &centroid)
{
    // Calculate centroid
    for (const auto& circle : circles)
    {
        centroid.setX(centroid.x() + circle.center.x());
        centroid.setY(centroid.y() + circle.center.y());
    }
    centroid.setX(centroid.x() / circles.size());
    centroid.setY(centroid.y() / circles.size());

    // Calculate distance and angle from centroid
    for (int i = 0; i < circles.size(); i++)
    {
        double dx = circles[i].center.x() - centroid.x();
        double dy = circles[i].center.y() - centroid.y();
        circles[i].distanceToCentroid = qSqrt(dx * dx + dy * dy);

        double angle = qAtan2(dy, dx) * 180.0 / M_PI;
        if (angle < 0) angle += 360.0;
        circles[i].angle = angle;
    }

    // Sort by distance to centroid
    std::sort(circles.begin(), circles.end(), [](const CircleInfo& a, const CircleInfo& b)
              {
                  return a.distanceToCentroid < b.distanceToCentroid;
              });

    // Validate the 5+11 arrangement
    QVector<double> distDifferences;
    for (int i = 1; i < circles.size(); i++)
    {
        distDifferences.append(circles[i].distanceToCentroid - circles[i-1].distanceToCentroid);
    }

    // Find largest jump in distance
    int largestJumpIndex = 0;
    double largestJump = distDifferences[0];
    for (int i = 1; i < distDifferences.size(); i++)
    {
        if (distDifferences[i] > largestJump)
        {
            largestJump = distDifferences[i];
            largestJumpIndex = i;
        }
    }

    // Calculate average inner circle distance
    double avgInnerDist = 0;
    for (int i = 0; i < 5 && i < circles.size(); i++)
    {
        avgInnerDist += circles[i].distanceToCentroid;
    }
    avgInnerDist /= qMin(5, circles.size());

    // Validate configuration
    bool validConfiguration = (largestJumpIndex == 4) && (largestJump > 0.2 * avgInnerDist);

    if (!validConfiguration)
    {
        return false;
    }

    // Separate inner and outer circles
    QVector<CircleInfo> innerCircles;
    QVector<CircleInfo> outerCircles;

    for (int i = 0; i < 5; i++)
    {
        innerCircles.append(circles[i]);
    }

    for (int i = 5; i < 16; i++)
    {
        outerCircles.append(circles[i]);
    }

    // Find reference line
    int innerRefIndex = -1;
    int outerRefIndex = -1;
    double bestAlignment = -1;

    for (int i = 0; i < innerCircles.size(); i++)
    {
        for (int j = 0; j < outerCircles.size(); j++)
        {
            double angleDiff = qAbs(innerCircles[i].angle - outerCircles[j].angle);
            if (angleDiff > 180) angleDiff = 360 - angleDiff;

            double alignment = 180 - angleDiff;

            if (alignment > bestAlignment)
            {
                bestAlignment = alignment;
                innerRefIndex = i;
                outerRefIndex = j;
            }
        }
    }

    if (innerRefIndex < 0 || outerRefIndex < 0)
    {
        return false;
    }

    // Assign circle IDs
    assignCircleIDs(innerCircles, outerCircles, innerRefIndex, outerRefIndex);

    // Rebuild the circles list
    circles.clear();
    for (const auto& circle : innerCircles)
    {
        circles.append(circle);
    }
    for (const auto& circle : outerCircles)
    {
        circles.append(circle);
    }

    return true;
}

void MainWindow::assignCircleIDs(QVector<CircleInfo> &innerCircles, QVector<CircleInfo> &outerCircles, int innerRefIndex, int outerRefIndex)
{
    // Set reference pair
    innerCircles[innerRefIndex].id = 1;
    outerCircles[outerRefIndex].id = 6;

    // Get reference angle
    double refAngle = innerCircles[innerRefIndex].angle;

    // Number inner circles (2-5) counterclockwise
    QVector<int> innerIndices;
    for (int i = 0; i < innerCircles.size(); i++)
    {
        if (i != innerRefIndex)
        {
            innerIndices.append(i);
        }
    }

    // Calculate relative angles
    for (int i = 0; i < innerIndices.size(); i++)
    {
        double relAngle = innerCircles[innerIndices[i]].angle - refAngle;
        if (relAngle < 0) relAngle += 360.0;
        innerCircles[innerIndices[i]].distanceToCentroid = relAngle; // Reuse field for sorting
    }

    // Sort by relative angle
    std::sort(innerIndices.begin(), innerIndices.end(), [&innerCircles](int a, int b)
              {
                  return innerCircles[a].distanceToCentroid > innerCircles[b].distanceToCentroid;
              });

    // Assign IDs 2-5
    for (int i = 0; i < innerIndices.size(); i++)
    {
        innerCircles[innerIndices[i]].id = i + 2;
    }

    // Number outer circles (7-16)
    QVector<int> outerIndices;
    for (int i = 0; i < outerCircles.size(); i++)
    {
        if (i != outerRefIndex)
        {
            outerIndices.append(i);
        }
    }

    // Calculate relative angles
    for (int i = 0; i < outerIndices.size(); i++)
    {
        double relAngle = outerCircles[outerIndices[i]].angle - refAngle;
        if (relAngle < 0) relAngle += 360.0;
        outerCircles[outerIndices[i]].distanceToCentroid = relAngle; // Reuse field for sorting
    }

    // Sort by relative angle
    std::sort(outerIndices.begin(), outerIndices.end(), [&outerCircles](int a, int b)
              {
                  return outerCircles[a].distanceToCentroid > outerCircles[b].distanceToCentroid;
              });

    // Assign IDs 7-16
    for (int i = 0; i < outerIndices.size(); i++)
    {
        outerCircles[outerIndices[i]].id = i + 7;
    }
}

void MainWindow::displayIdentifiedCircles(QVector<CircleInfo> &circles, QPointF centroid)
{
    qDebug() << "Starting displayIdentifiedCircles with" << circles.size() << "circles";

    QGraphicsScene *scene = ui->graphicsViewCamera->scene();
    QGraphicsPixmapItem *pixmapItem = qgraphicsitem_cast<QGraphicsPixmapItem*>(scene->items().first());

    if (!pixmapItem)
    {
        qDebug() << "Error: First item is not a QGraphicsPixmapItem.";
        return;
    }

    QPixmap pixmap = pixmapItem->pixmap();
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw all circles with their numbers
    for (const auto& circleInfo : circles)
    {
        int x = circleInfo.center.x();
        int y = circleInfo.center.y();
        int radius = circleInfo.radius;

        qDebug() << "Drawing circle" << circleInfo.id << "at" << x << "," << y;

        // Draw circle in red
        painter.setPen(QPen(Qt::red, 2));
        painter.drawEllipse(QPointF(x, y), radius, radius);

        // Draw circle number
        QFont font = painter.font();
        font.setPointSize(18);
        font.setBold(true);
        painter.setFont(font);

        double angle = 45.0;
        double textX = x + (radius + 10) * qCos(angle * M_PI / 180.0);
        double textY = y - (radius + 10) * qSin(angle * M_PI / 180.0);

        // Draw text with outline for visibility
        painter.setPen(QPen(Qt::black, 3));
        painter.drawText(QPointF(textX-1, textY-1), QString::number(circleInfo.id));
        painter.drawText(QPointF(textX+1, textY-1), QString::number(circleInfo.id));
        painter.drawText(QPointF(textX-1, textY+1), QString::number(circleInfo.id));
        painter.drawText(QPointF(textX+1, textY+1), QString::number(circleInfo.id));

        painter.setPen(QPen(Qt::blue, 1));
        painter.drawText(QPointF(textX, textY), QString::number(circleInfo.id));
    }

    // Draw centroid
    painter.setPen(QPen(Qt::yellow, 3));
    painter.drawLine(centroid.x() - 20, centroid.y(), centroid.x() + 20, centroid.y());
    painter.drawLine(centroid.x(), centroid.y() - 20, centroid.x(), centroid.y() + 20);

    // Display the numbered circles
    scene->clear();
    scene->addPixmap(pixmap);

    qDebug() << "Finished displaying identified circles";

    // Process events to keep UI responsive
    QApplication::processEvents();
}

void MainWindow::updateCircleDebugView(const CircleInfo &circleInfo, const QImage &processedImage, bool found, const QString &code)
{

    QGraphicsView* debugView = nullptr;
    switch (circleInfo.id)
    {
    case 1: debugView = ui->graphicsViewCircle_1; break;
    case 2: debugView = ui->graphicsViewCircle_2; break;
    case 3: debugView = ui->graphicsViewCircle_3; break;
    case 4: debugView = ui->graphicsViewCircle_4; break;
    case 5: debugView = ui->graphicsViewCircle_5; break;
    case 6: debugView = ui->graphicsViewCircle_6; break;
    case 7: debugView = ui->graphicsViewCircle_7; break;
    case 8: debugView = ui->graphicsViewCircle_8; break;
    case 9: debugView = ui->graphicsViewCircle_9; break;
    case 10: debugView = ui->graphicsViewCircle_10; break;
    case 11: debugView = ui->graphicsViewCircle_11; break;
    case 12: debugView = ui->graphicsViewCircle_12; break;
    case 13: debugView = ui->graphicsViewCircle_13; break;
    case 14: debugView = ui->graphicsViewCircle_14; break;
    case 15: debugView = ui->graphicsViewCircle_15; break;
    case 16: debugView = ui->graphicsViewCircle_16; break;
    }

    if (debugView)
    {

        QGraphicsScene *scene = new QGraphicsScene(debugView);
        scene->addPixmap(QPixmap::fromImage(processedImage));

        if (found)
        {
            QGraphicsTextItem* foundText = scene->addText("Found");
            foundText->setDefaultTextColor(Qt::green);
            foundText->setPos(5, 5);

            QGraphicsTextItem* codeText = scene->addText(code);
            codeText->setDefaultTextColor(Qt::green);
            codeText->setPos(5, processedImage.height() - 20);
        }
        else
        {
            QGraphicsTextItem* searchingText = scene->addText("Searching");
            searchingText->setDefaultTextColor(Qt::red);
            searchingText->setPos(5, 5);
        }

        debugView->setScene(scene);
    }
    else
    {
        qDebug() << "Error: No debug view found for circle ID" << circleInfo.id;
    }
}

ImageProcessor::ImageProcessor(QObject *parent) : QObject(parent)
{
    // Initialization if needed
}

ImageProcessor::~ImageProcessor()
{
    // Clean-up if needed
}

void ImageProcessor::processImage(const QImage &image, const QVector<CircleInfo> &circles, const QSet<int> &processedCircles)
{
    if (image.isNull()) return;

    QImage enhanced = image.convertToFormat(QImage::Format_Grayscale8);
    int processedCount = 0;

    for (const CircleInfo &circleInfo : circles)
    {
        if (processedCircles.contains(circleInfo.id)) {
            continue;
        }

        QImage processedImage = processCircleRegion(enhanced, circleInfo);
        emit debugImage(circleInfo.id, processedImage);

        CircleInfo tempCircle = circleInfo; // Create a copy to modify
        if (detectDataMatrixInCircle(processedImage, tempCircle))
        {
            emit dataMatrixFound(circleInfo.id, tempCircle.dmCode, processedImage);
            processedCount++;
        }
    }

    emit processingComplete(processedCount);
}

QImage ImageProcessor::processCircleRegion(const QImage &sourceImage, const CircleInfo &circleInfo)
{
    int x = circleInfo.center.x();
    int y = circleInfo.center.y();
    int radius = circleInfo.radius;

    // Extract region around this circle
    int startX = qMax(0, x - radius);
    int startY = qMax(0, y - radius);
    int endX = qMin(sourceImage.width() - 1, x + radius);
    int endY = qMin(sourceImage.height() - 1, y + radius);
    int width = endX - startX;
    int height = endY - startY;

    if (width <= 0 || height <= 0)
    {
        return QImage(); // Return empty image if dimensions invalid
    }

    // Create masked circle region
    QImage circleRegion(width, height, QImage::Format_ARGB32);
    circleRegion.fill(Qt::transparent);

    QPainter circlePainter(&circleRegion);
    circlePainter.setRenderHint(QPainter::Antialiasing);
    circlePainter.setPen(Qt::NoPen);
    circlePainter.setBrush(Qt::white);
    circlePainter.drawEllipse(radius - (x - startX), radius - (y - startY), radius * 2, radius * 2);

    circlePainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    circlePainter.drawImage(0, 0, sourceImage, startX, startY);
    circlePainter.end();

    // Apply blur effect
    QImage blurredCircle = circleRegion.copy();
    {
        QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect();
        blurEffect->setBlurRadius(12.0);

        QGraphicsScene scene;
        QGraphicsPixmapItem item;
        item.setPixmap(QPixmap::fromImage(blurredCircle));
        item.setGraphicsEffect(blurEffect);
        scene.addItem(&item);
        QPainter painter(&blurredCircle);
        scene.render(&painter);
        painter.end();

        // Clean up the blur effect
        delete blurEffect;
    }

    // Apply sharpening
    {
        double amount = 2.5;
        for (int y = 0; y < circleRegion.height(); y++)
        {
            for (int x = 0; x < circleRegion.width(); x++)
            {
                int originalVal = qGray(circleRegion.pixel(x, y));
                int blurredVal = qGray(blurredCircle.pixel(x, y));

                int sharpened = qBound(0, (int)(originalVal + amount * (originalVal - blurredVal)), 255);

                circleRegion.setPixel(x, y, qRgb(sharpened, sharpened, sharpened));
            }
        }
    }

    return circleRegion;
}

bool ImageProcessor::detectDataMatrixInCircle(const QImage &processedImage, CircleInfo &circleInfo)
{
    if (processedImage.isNull())
    {
        return false;
    }

    ZXing::DecodeHints hints;
    hints.setTryHarder(true);
    hints.setTryRotate(true);
    hints.setFormats(ZXing::BarcodeFormat::DataMatrix);

    ZXing::ImageView regionView(processedImage.constBits(),
                                processedImage.width(),
                                processedImage.height(),
                                ZXing::ImageFormat::Lum);

    std::vector<ZXing::Result> results = ZXing::ReadBarcodes(regionView, hints);

    for (const auto& result : results)
    {
        if (result.isValid())
        {
            QString code = QString::fromStdString(result.text());

            qDebug() << "DM code found:" << code << "in circle" << circleInfo.id;

            // Mark circle as having a DM code
            circleInfo.hasDM = true;
            circleInfo.dmCode = code;

            return true;
        }
    }

    return false;
}

QImage MainWindow::processCircleRegion(const QImage &sourceImage, const CircleInfo &circleInfo)
{
    int x = circleInfo.center.x();
    int y = circleInfo.center.y();
    int radius = circleInfo.radius;

    // Extract region around this circle
    int startX = qMax(0, x - radius);
    int startY = qMax(0, y - radius);
    int endX = qMin(sourceImage.width() - 1, x + radius);
    int endY = qMin(sourceImage.height() - 1, y + radius);
    int width = endX - startX;
    int height = endY - startY;

    if (width <= 0 || height <= 0)
    {
        qDebug() << "Invalid dimensions for circle" << circleInfo.id;
        return QImage();
    }

    // Simply extract the rectangle for now - no masking
    QImage directExtract = sourceImage.copy(startX, startY, width, height);

    // Draw a border to make it obvious
    QPainter painter(&directExtract);
    painter.setPen(QPen(Qt::white, 3));
    painter.drawRect(0, 0, width-1, height-1);
    painter.end();

    return directExtract;
}

bool MainWindow::detectDataMatrices(QImageCapture *imageCapture, QVector<CircleInfo> &circles)
{
    QElapsedTimer dmTimer;
    dmTimer.start();
    int iterationCount = 0;
    QSet<int> detectedCircleIDs;
    qDebug() << "Before loop - detectedCircleIDs size:" << detectedCircleIDs.size()
             << "timer started:" << dmTimer.isValid();
    qDebug() << "Starting DataMatrix detection+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";



    while (detectedCircleIDs.size() < 16 && dmTimer.elapsed() < 20000)
    {
        iterationCount++;
        qDebug() << "Iteration:" << iterationCount << "Elapsed Time:" << dmTimer.elapsed() << "ms";

        // Capture a single image
        QImage captured;
        QEventLoop loop;
        auto connection = connect(imageCapture, &QImageCapture::imageCaptured,
                                  [&captured, &loop](int id, const QImage &img) {
                                      captured = img;
                                      loop.quit();
                                  });

        imageCapture->capture();
        loop.exec();
        disconnect(connection);

        if (captured.isNull()) {
            qDebug() << "Failed to capture image";
            continue;
        }

        // Process each circle that hasn't been detected yet
        for (int i = 0; i < circles.size(); i++) {
            if (detectedCircleIDs.contains(circles[i].id)) {
                continue; // Skip already detected circles
            }

            QImage region = processCircleRegion(captured, circles[i]);

            if (!region.isNull()) {
                updateCircleDebugView(circles[i], region);

                // You can add DataMatrix detection here when ready
            }
        }

        QApplication::processEvents();
        QThread::msleep(50); // Short delay between iterations
    }

    qDebug() << "DataMatrix detection complete. Found" << detectedCircleIDs.size()
             << "codes in" << dmTimer.elapsed() / 1000.0 << "seconds";

    return detectedCircleIDs.size() == 16;
}
