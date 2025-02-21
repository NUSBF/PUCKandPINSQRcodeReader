/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QtWidgets/QApplication>
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
        pushButtonCamera1->setGeometry(QRect(150, 40, 80, 25));
        pushButtonCamera2 = new QPushButton(centralwidget);
        pushButtonCamera2->setObjectName("pushButtonCamera2");
        pushButtonCamera2->setGeometry(QRect(1150, 40, 80, 25));
        widgetCamera1 = new QVideoWidget(centralwidget);
        widgetCamera1->setObjectName("widgetCamera1");
        widgetCamera1->setGeometry(QRect(80, 160, 681, 651));
        widgetCamera2 = new QVideoWidget(centralwidget);
        widgetCamera2->setObjectName("widgetCamera2");
        widgetCamera2->setGeometry(QRect(920, 140, 781, 741));
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
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
