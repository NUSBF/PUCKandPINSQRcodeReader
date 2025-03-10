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
    detectQRCodeFromCameraCircle(camera2,16);

}

void MainWindow::on_pushButtonCamera1_clicked()
{
    // Start the camera feed
    camera1->start();
    detectQRCodeFromCamera(camera1,1);

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

void MainWindow::detectQRCodeFromCamera(QCamera *camera, int targetDMCount)
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
            [this, sharedState, targetDMCount, captureTimer, imageCapture](int id, const QImage &originalImage) {
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
                if (this->dmDetectionTimer.elapsed() < 10000 && sharedState->detectedDMs.size() < targetDMCount) {
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

void MainWindow::detectQRCodeFromCameraCircle(QCamera *camera, int targetDMCount)
{
    if (!ui->graphicsViewCamera->scene()) {
        ui->graphicsViewCamera->setScene(new QGraphicsScene(this));
    }

    QMediaCaptureSession *currentSession = (camera == camera1) ? captureSession1 : captureSession2;
    QImageCapture *imageCapture = new QImageCapture(this);
    currentSession->setImageCapture(imageCapture);

    // Structure to store circle information
    struct CircleInfo {
        int id;
        QPointF center;
        int radius;
        QString dmCode;
        bool hasDM;
        double distanceToCentroid;
    };

    // Create shared state
    QVector<CircleInfo> *circles = new QVector<CircleInfo>();
    QSet<QString> *detectedDMs = new QSet<QString>();
    bool *circlesIdentified = new bool(false);

    this->dmDetectionTimer.start();
    QTimer *captureTimer = new QTimer(this);

    connect(imageCapture, &QImageCapture::imageCaptured, this,
            [this, circles, detectedDMs, circlesIdentified, targetDMCount, captureTimer, imageCapture](int id, const QImage &originalImage) {
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

                // Create a colored image for visualization
                QImage displayImage = enhanced.convertToFormat(QImage::Format_RGB32);
                QPainter displayPainter(&displayImage);
                displayPainter.setRenderHint(QPainter::Antialiasing);

                // If circles haven't been identified yet, detect them
                if (!(*circlesIdentified)) {
                    // Convert enhanced QImage to OpenCV Mat
                    cv::Mat enhancedMat(enhanced.height(), enhanced.width(), CV_8UC1,
                                        const_cast<uchar*>(enhanced.constBits()), enhanced.bytesPerLine());

                    // Detect circles using HoughCircles
                    std::vector<cv::Vec3f> detectedCircles;
                    cv::HoughCircles(enhancedMat, detectedCircles, cv::HOUGH_GRADIENT, 1,
                                     enhancedMat.rows/20,
                                     100, 40,
                                     40, 50);

                    // Take up to 16 circles
                    int count = 0;
                    for (const auto& circle : detectedCircles) {
                        if (count >= 16) break;

                        CircleInfo info;
                        info.id = 0; // Will be assigned later
                        info.center = QPointF(cvRound(circle[0]), cvRound(circle[1]));
                        info.radius = cvRound(circle[2]);
                        info.hasDM = false;

                        circles->append(info);
                        count++;
                    }

                    // Calculate centroid
                    QPointF centroid(0, 0);
                    for (const auto& circle : *circles) {
                        centroid += circle.center;
                    }
                    centroid /= circles->size();

                    // First find the closest circle to centroid (Circle 1)
                    double minDistance = std::numeric_limits<double>::max();
                    int circle1Index = 0;
                    for (int i = 0; i < circles->size(); i++) {
                        QPointF diff = (*circles)[i].center - centroid;
                        double distance = sqrt(diff.x() * diff.x() + diff.y() * diff.y());
                        (*circles)[i].distanceToCentroid = distance;

                        if (distance < minDistance) {
                            minDistance = distance;
                            circle1Index = i;
                        }
                    }
                    // Assign ID 1 to the closest circle
                    (*circles)[circle1Index].id = 1;

                    // Find circle most aligned with Circle 1 and centroid, but further away
                    QPointF c1Direction = (*circles)[circle1Index].center - centroid;
                    double c1Length = sqrt(c1Direction.x() * c1Direction.x() + c1Direction.y() * c1Direction.y());
                    if (c1Length > 0) {
                        c1Direction.rx() /= c1Length;
                        c1Direction.ry() /= c1Length;

                        // Find the circle best aligned with this direction and further than Circle 1
                        double bestAlignment = -1.0; // Best dot product (alignment)
                        int circle6Index = -1;

                        for (int i = 0; i < circles->size(); i++) {
                            if (i == circle1Index) continue; // Skip Circle 1

                            QPointF circleDirection = (*circles)[i].center - centroid;
                            double circleLength = sqrt(circleDirection.x() * circleDirection.x() +
                                                       circleDirection.y() * circleDirection.y());

                            if (circleLength > c1Length) { // Only consider circles further than Circle 1
                                // Normalize
                                circleDirection.rx() /= circleLength;
                                circleDirection.ry() /= circleLength;

                                // Calculate dot product (measure of alignment)
                                double dotProduct = c1Direction.x() * circleDirection.x() +
                                                    c1Direction.y() * circleDirection.y();

                                // We want the most aligned circle (highest dot product)
                                if (dotProduct > bestAlignment) {
                                    bestAlignment = dotProduct;
                                    circle6Index = i;
                                }
                            }
                        }

                        // If we found a suitable Circle 6, assign its ID
                        if (circle6Index >= 0) {
                            (*circles)[circle6Index].id = 6;
                        }
                        else {
                            // If no suitable circle found, just use the furthest circle
                            double maxDistance = 0;
                            for (int i = 0; i < circles->size(); i++) {
                                if (i != circle1Index && (*circles)[i].distanceToCentroid > maxDistance) {
                                    maxDistance = (*circles)[i].distanceToCentroid;
                                    circle6Index = i;
                                }
                            }
                            (*circles)[circle6Index].id = 6;
                        }

                        // Number circles 2-5 counter-clockwise from circle 1 (closest to centroid)
                        // Get the angle of circle 1 from centroid
                        double circle1Angle = atan2(c1Direction.y(), c1Direction.x());

                        // Create a list of remaining circles (not 1 or 6)
                        QVector<int> remainingIndices;
                        QVector<double> angles;
                        for (int i = 0; i < circles->size(); i++) {
                            if (i != circle1Index && i != circle6Index) {
                                QPointF dir = (*circles)[i].center - centroid;
                                double len = sqrt(dir.x() * dir.x() + dir.y() * dir.y());

                                if (len > 0) {
                                    double angle = atan2(dir.y(), dir.x()) - circle1Angle;
                                    // Normalize angle to [0, 2π)
                                    while (angle < 0) angle += 2 * M_PI;
                                    while (angle >= 2 * M_PI) angle -= 2 * M_PI;

                                    remainingIndices.push_back(i);
                                    angles.push_back(angle);
                                }
                            }
                        }

                        // Sort circles by angle
                        for (int i = 0; i < remainingIndices.size(); i++) {
                            for (int j = i + 1; j < remainingIndices.size(); j++) {
                                if (angles[i] > angles[j]) {
                                    std::swap(angles[i], angles[j]);
                                    std::swap(remainingIndices[i], remainingIndices[j]);
                                }
                            }
                        }

                        // Assign IDs 2-5 to the first 4 circles (or fewer if we have fewer circles)
                        for (int i = 0; i < (remainingIndices.size() < 4 ? remainingIndices.size() : 4); i++) {
                            (*circles)[remainingIndices[i]].id = i + 2;
                        }

                        // Get the angle of circle 6 from centroid
                        QPointF c6Direction = (*circles)[circle6Index].center - centroid;
                        double circle6Angle = atan2(c6Direction.y(), c6Direction.x());

                        // Create a list of remaining circles (those without IDs)
                        remainingIndices.clear();
                        angles.clear();
                        for (int i = 0; i < circles->size(); i++) {
                            if ((*circles)[i].id == 0) {
                                QPointF dir = (*circles)[i].center - centroid;
                                double len = sqrt(dir.x() * dir.x() + dir.y() * dir.y());

                                if (len > 0) {
                                    double angle = atan2(dir.y(), dir.x()) - circle6Angle;
                                    // Normalize angle to [0, 2π)
                                    while (angle < 0) angle += 2 * M_PI;
                                    while (angle >= 2 * M_PI) angle -= 2 * M_PI;

                                    remainingIndices.push_back(i);
                                    angles.push_back(angle);
                                }
                            }
                        }

                        // Sort circles by angle
                        for (int i = 0; i < remainingIndices.size(); i++) {
                            for (int j = i + 1; j < remainingIndices.size(); j++) {
                                if (angles[i] > angles[j]) {
                                    std::swap(angles[i], angles[j]);
                                    std::swap(remainingIndices[i], remainingIndices[j]);
                                }
                            }
                        }

                        // Assign IDs 7-16 to the remaining circles
                        for (int i = 0; i < remainingIndices.size(); i++) {
                            (*circles)[remainingIndices[i]].id = i + 7;
                        }
                    }

                    *circlesIdentified = true;
                    qDebug() << "Identified" << circles->size() << "circles";
                }

                // Calculate center of mass for all circles
                QPointF centerOfMass(0, 0);
                if (circles->size() > 0) {
                    for (const auto& circle : *circles) {
                        centerOfMass += circle.center;
                    }
                    centerOfMass /= circles->size();
                }

                // Now process each identified circle for DM codes
                for (int i = 0; i < circles->size(); i++) {
                    CircleInfo& circleInfo = (*circles)[i];
                    int x = circleInfo.center.x();
                    int y = circleInfo.center.y();
                    int radius = circleInfo.radius;

                    // Draw the circle in red
                    displayPainter.setPen(QPen(Qt::red, 2));
                    displayPainter.drawEllipse(QPointF(x, y), radius, radius);

                    // Draw the circle number (blue text with black outline for visibility)
                    QFont font = displayPainter.font();
                    font.setPointSize(18);  // Larger font size
                    font.setBold(true);
                    displayPainter.setFont(font);

                    // Draw text just outside the circle
                    // Calculate position for the number (outside the circle)
                    double angle = 45.0; // Position at 45 degrees (top-right)
                    double textX = x + (radius + 10) * cos(angle * M_PI / 180.0);
                    double textY = y - (radius + 10) * sin(angle * M_PI / 180.0);

                    // Draw text with outline for better visibility
                    displayPainter.setPen(QPen(Qt::black, 3));
                    displayPainter.drawText(QPointF(textX-1, textY-1), QString::number(circleInfo.id));
                    displayPainter.drawText(QPointF(textX+1, textY-1), QString::number(circleInfo.id));
                    displayPainter.drawText(QPointF(textX-1, textY+1), QString::number(circleInfo.id));
                    displayPainter.drawText(QPointF(textX+1, textY+1), QString::number(circleInfo.id));

                    displayPainter.setPen(QPen(Qt::blue, 1));  // Blue text
                    displayPainter.drawText(QPointF(textX, textY), QString::number(circleInfo.id));

                    // Only process for DM codes if we haven't found one in this circle yet
                    if (!circleInfo.hasDM) {
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

                        for (const auto& result : results) {
                            if (result.isValid()) {
                                QString code = QString::fromStdString(result.text());

                                if (!detectedDMs->contains(code)) {
                                    detectedDMs->insert(code);
                                    qDebug() << "New DM code found:" << code << "in circle" << circleInfo.id;
                                }

                                // Mark this circle as having a DM code
                                circleInfo.hasDM = true;
                                circleInfo.dmCode = code;

                                // Draw circle in green instead of red if DM found
                                displayPainter.setPen(QPen(Qt::green, 2));
                                displayPainter.drawEllipse(QPointF(x, y), radius, radius);

                                // Draw the code text
                                displayPainter.setPen(Qt::green);
                                font.setPointSize(10);
                                displayPainter.setFont(font);
                                displayPainter.drawText(
                                    QPoint(x - 20, y + radius + 20),
                                    code);

                                // Draw rectangle for the detected code
                                auto points = result.position();
                                displayPainter.setPen(QPen(Qt::green, 2));
                                displayPainter.drawRect(
                                    startX + points.topLeft().x,
                                    startY + points.topLeft().y,
                                    points.bottomRight().x - points.topLeft().x,
                                    points.bottomRight().y - points.topLeft().y);

                                break;
                            }
                        }
                    } else {
                        // If circle already has a DM code, draw it in green
                        displayPainter.setPen(QPen(Qt::green, 2));
                        displayPainter.drawEllipse(QPointF(x, y), radius, radius);

                        // Draw the code text
                        displayPainter.setPen(Qt::green);
                        font.setPointSize(10);
                        displayPainter.setFont(font);
                        displayPainter.drawText(
                            QPoint(x - 20, y + radius + 20),
                            circleInfo.dmCode);
                    }
                }

                // Draw center of mass with a cross if circles were detected
                if (circles->size() > 0) {
                    // Draw a prominent cross at the center of mass
                    int crossSize = 20;
                    displayPainter.setPen(QPen(Qt::yellow, 3));

                    // Horizontal line
                    displayPainter.drawLine(
                        centerOfMass.x() - crossSize, centerOfMass.y(),
                        centerOfMass.x() + crossSize, centerOfMass.y()
                        );

                    // Vertical line
                    displayPainter.drawLine(
                        centerOfMass.x(), centerOfMass.y() - crossSize,
                        centerOfMass.x(), centerOfMass.y() + crossSize
                        );

                    // Optional: Draw a small circle at the intersection
                    displayPainter.drawEllipse(centerOfMass, 5, 5);

                    // Add text label for the center of mass
                    QFont font = displayPainter.font();
                    font.setPointSize(10);
                    font.setBold(true);
                    displayPainter.setFont(font);
                    displayPainter.setPen(Qt::yellow);
                    displayPainter.drawText(
                        QPointF(centerOfMass.x() + 25, centerOfMass.y() - 10),
                        "Center of Mass"
                        );

                    // Display the coordinates
                    displayPainter.drawText(
                        QPointF(centerOfMass.x() + 25, centerOfMass.y() + 10),
                        QString("(%1, %2)").arg(int(centerOfMass.x())).arg(int(centerOfMass.y()))
                        );

                    // Find circles 1 and 6 for debugging line
                    CircleInfo* circle1 = nullptr;
                    CircleInfo* circle6 = nullptr;
                    for (int i = 0; i < circles->size(); i++) {
                        if ((*circles)[i].id == 1) {
                            circle1 = &(*circles)[i];
                        }
                        else if ((*circles)[i].id == 6) {
                            circle6 = &(*circles)[i];
                        }
                    }
                    // Draw a debug line if both circles are identified
                    if (circle1 && circle6) {
                        // Calculate direction vector from centroid to circle1
                        QPointF direction = circle1->center - centerOfMass;

                        // Normalize the direction vector
                        double length = sqrt(direction.x() * direction.x() + direction.y() * direction.y());
                        if (length > 0) {
                            direction.rx() /= length;
                            direction.ry() /= length;

                            // Calculate point far in one direction (from centroid outward)
                            int maxDimension = std::max(displayImage.width(), displayImage.height()) * 2;
                            QPointF farPoint = centerOfMass + direction * maxDimension;

                            // Draw line from centroid to far point (a radius, not diameter)
                            displayPainter.setPen(QPen(Qt::cyan, 2, Qt::DashLine));
                            displayPainter.drawLine(centerOfMass, farPoint);
                        }
                    }

                }

                displayPainter.end();

                // Display the resulting image
                QPixmap resultPixmap = QPixmap::fromImage(displayImage);
                ui->graphicsViewCamera->scene()->clear();
                ui->graphicsViewCamera->scene()->addPixmap(resultPixmap);

                // Add text showing counts
                QGraphicsTextItem* text = ui->graphicsViewCamera->scene()->addText(
                    QString("Identified %1 circles | Detected %2 DM codes | Time: %3 sec")
                        .arg(circles->size())
                        .arg(detectedDMs->size())
                        .arg(this->dmDetectionTimer.elapsed() / 1000.0, 0, 'f', 1));
                text->setDefaultTextColor(Qt::white);
                text->setPos(10, 10);

                // Check if we should continue
                if (this->dmDetectionTimer.elapsed() < 10000 && detectedDMs->size() < targetDMCount) {
                    // Continue detection
                } else {
                    qDebug() << "Detection complete. Found" << detectedDMs->size() << "unique DM codes in"
                             << this->dmDetectionTimer.elapsed() / 1000.0 << "seconds.";

                    captureTimer->stop();
                    captureTimer->deleteLater();
                    imageCapture->deleteLater();
                    delete circles;
                    delete detectedDMs;
                    delete circlesIdentified;
                }
            });

    connect(captureTimer, &QTimer::timeout, this, [imageCapture]() {
        imageCapture->capture();
    });

    captureTimer->start(200);
    imageCapture->capture();
}
