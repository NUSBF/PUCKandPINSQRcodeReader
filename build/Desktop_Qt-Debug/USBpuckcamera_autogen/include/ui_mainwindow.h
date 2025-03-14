/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *pushButtonCamera1;
    QPushButton *pushButtonCamera2;
    QVideoWidget *widgetCamera1;
    QVideoWidget *widgetCamera2;
    QGraphicsView *graphicsViewCamera;
    QPushButton *pushButtonTestQRCodDetection;
    QGraphicsView *graphicsViewCircle_1;
    QGraphicsView *graphicsViewCircle_2;
    QGraphicsView *graphicsViewCircle_3;
    QGraphicsView *graphicsViewCircle_4;
    QGraphicsView *graphicsViewCircle_5;
    QGraphicsView *graphicsViewCircle_6;
    QGraphicsView *graphicsViewCircle_7;
    QGraphicsView *graphicsViewCircle_8;
    QGraphicsView *graphicsViewCircle_9;
    QGraphicsView *graphicsViewCircle_10;
    QGraphicsView *graphicsViewCircle_11;
    QGraphicsView *graphicsViewCircle_12;
    QGraphicsView *graphicsViewCircle_13;
    QGraphicsView *graphicsViewCircle_14;
    QGraphicsView *graphicsViewCircle_15;
    QGraphicsView *graphicsViewCircle_16;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1788, 963);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        pushButtonCamera1 = new QPushButton(centralwidget);
        pushButtonCamera1->setObjectName("pushButtonCamera1");
        pushButtonCamera1->setGeometry(QRect(10, 10, 80, 25));
        pushButtonCamera2 = new QPushButton(centralwidget);
        pushButtonCamera2->setObjectName("pushButtonCamera2");
        pushButtonCamera2->setGeometry(QRect(10, 390, 80, 25));
        widgetCamera1 = new QVideoWidget(centralwidget);
        widgetCamera1->setObjectName("widgetCamera1");
        widgetCamera1->setGeometry(QRect(10, 40, 361, 341));
        widgetCamera2 = new QVideoWidget(centralwidget);
        widgetCamera2->setObjectName("widgetCamera2");
        widgetCamera2->setGeometry(QRect(10, 420, 521, 491));
        graphicsViewCamera = new QGraphicsView(centralwidget);
        graphicsViewCamera->setObjectName("graphicsViewCamera");
        graphicsViewCamera->setGeometry(QRect(550, 40, 1001, 871));
        pushButtonTestQRCodDetection = new QPushButton(centralwidget);
        pushButtonTestQRCodDetection->setObjectName("pushButtonTestQRCodDetection");
        pushButtonTestQRCodDetection->setGeometry(QRect(210, 10, 161, 25));
        graphicsViewCircle_1 = new QGraphicsView(centralwidget);
        graphicsViewCircle_1->setObjectName("graphicsViewCircle_1");
        graphicsViewCircle_1->setGeometry(QRect(1560, 40, 100, 100));
        graphicsViewCircle_2 = new QGraphicsView(centralwidget);
        graphicsViewCircle_2->setObjectName("graphicsViewCircle_2");
        graphicsViewCircle_2->setGeometry(QRect(1560, 150, 100, 100));
        graphicsViewCircle_3 = new QGraphicsView(centralwidget);
        graphicsViewCircle_3->setObjectName("graphicsViewCircle_3");
        graphicsViewCircle_3->setGeometry(QRect(1560, 260, 100, 100));
        graphicsViewCircle_4 = new QGraphicsView(centralwidget);
        graphicsViewCircle_4->setObjectName("graphicsViewCircle_4");
        graphicsViewCircle_4->setGeometry(QRect(1560, 370, 100, 100));
        graphicsViewCircle_5 = new QGraphicsView(centralwidget);
        graphicsViewCircle_5->setObjectName("graphicsViewCircle_5");
        graphicsViewCircle_5->setGeometry(QRect(1560, 480, 100, 100));
        graphicsViewCircle_6 = new QGraphicsView(centralwidget);
        graphicsViewCircle_6->setObjectName("graphicsViewCircle_6");
        graphicsViewCircle_6->setGeometry(QRect(1560, 590, 100, 100));
        graphicsViewCircle_7 = new QGraphicsView(centralwidget);
        graphicsViewCircle_7->setObjectName("graphicsViewCircle_7");
        graphicsViewCircle_7->setGeometry(QRect(1560, 700, 100, 100));
        graphicsViewCircle_8 = new QGraphicsView(centralwidget);
        graphicsViewCircle_8->setObjectName("graphicsViewCircle_8");
        graphicsViewCircle_8->setGeometry(QRect(1560, 810, 100, 100));
        graphicsViewCircle_9 = new QGraphicsView(centralwidget);
        graphicsViewCircle_9->setObjectName("graphicsViewCircle_9");
        graphicsViewCircle_9->setGeometry(QRect(1670, 40, 100, 100));
        graphicsViewCircle_10 = new QGraphicsView(centralwidget);
        graphicsViewCircle_10->setObjectName("graphicsViewCircle_10");
        graphicsViewCircle_10->setGeometry(QRect(1670, 150, 100, 100));
        graphicsViewCircle_11 = new QGraphicsView(centralwidget);
        graphicsViewCircle_11->setObjectName("graphicsViewCircle_11");
        graphicsViewCircle_11->setGeometry(QRect(1670, 260, 100, 100));
        graphicsViewCircle_12 = new QGraphicsView(centralwidget);
        graphicsViewCircle_12->setObjectName("graphicsViewCircle_12");
        graphicsViewCircle_12->setGeometry(QRect(1670, 370, 100, 100));
        graphicsViewCircle_13 = new QGraphicsView(centralwidget);
        graphicsViewCircle_13->setObjectName("graphicsViewCircle_13");
        graphicsViewCircle_13->setGeometry(QRect(1670, 480, 100, 100));
        graphicsViewCircle_14 = new QGraphicsView(centralwidget);
        graphicsViewCircle_14->setObjectName("graphicsViewCircle_14");
        graphicsViewCircle_14->setGeometry(QRect(1670, 590, 100, 100));
        graphicsViewCircle_15 = new QGraphicsView(centralwidget);
        graphicsViewCircle_15->setObjectName("graphicsViewCircle_15");
        graphicsViewCircle_15->setGeometry(QRect(1670, 700, 100, 100));
        graphicsViewCircle_16 = new QGraphicsView(centralwidget);
        graphicsViewCircle_16->setObjectName("graphicsViewCircle_16");
        graphicsViewCircle_16->setGeometry(QRect(1670, 810, 100, 100));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1788, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        pushButtonCamera1->setText(QCoreApplication::translate("MainWindow", "Camera1", nullptr));
        pushButtonCamera2->setText(QCoreApplication::translate("MainWindow", "Camera2", nullptr));
        pushButtonTestQRCodDetection->setText(QCoreApplication::translate("MainWindow", "Test QR code detection", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
