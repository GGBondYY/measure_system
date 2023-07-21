/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_3;
    QCustomPlot *customPlot_two;
    QCustomPlot *fft_two;
    QCustomPlot *fft_one;
    QCustomPlot *customPlot_one;
    QCustomPlot *axisOrbit;
    QWidget *widget;
    QGridLayout *gridLayout_4;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QLabel *label_3;
    QLabel *label_2;
    QLabel *label_4;
    QComboBox *baudRate;
    QComboBox *portSelect;
    QLabel *label;
    QComboBox *stopBits;
    QPushButton *openPort;
    QComboBox *dataBits;
    QLabel *label_6;
    QLabel *label_5;
    QComboBox *parity;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout;
    QPushButton *connect_db;
    QLabel *label_8;
    QPushButton *start_collect;
    QLineEdit *freq;
    QLabel *label_7;
    QLineEdit *freq_num;
    QPushButton *hanning;
    QPushButton *hamming;
    QPushButton *export_data;
    QPushButton *clear_db;
    QMenuBar *menubar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1027, 662);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        QFont font;
        font.setPointSize(12);
        groupBox_2->setFont(font);
        gridLayout_3 = new QGridLayout(groupBox_2);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        customPlot_two = new QCustomPlot(groupBox_2);
        customPlot_two->setObjectName(QString::fromUtf8("customPlot_two"));

        gridLayout_3->addWidget(customPlot_two, 1, 0, 1, 1);

        fft_two = new QCustomPlot(groupBox_2);
        fft_two->setObjectName(QString::fromUtf8("fft_two"));

        gridLayout_3->addWidget(fft_two, 1, 1, 1, 1);

        fft_one = new QCustomPlot(groupBox_2);
        fft_one->setObjectName(QString::fromUtf8("fft_one"));

        gridLayout_3->addWidget(fft_one, 0, 1, 1, 1);

        customPlot_one = new QCustomPlot(groupBox_2);
        customPlot_one->setObjectName(QString::fromUtf8("customPlot_one"));

        gridLayout_3->addWidget(customPlot_one, 0, 0, 1, 1);

        axisOrbit = new QCustomPlot(groupBox_2);
        axisOrbit->setObjectName(QString::fromUtf8("axisOrbit"));
        axisOrbit->setFont(font);

        gridLayout_3->addWidget(axisOrbit, 0, 2, 2, 1);


        verticalLayout->addWidget(groupBox_2);

        widget = new QWidget(centralwidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        gridLayout_4 = new QGridLayout(widget);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        groupBox = new QGroupBox(widget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        QFont font1;
        font1.setPointSize(11);
        groupBox->setFont(font1);
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setFont(font);

        gridLayout_2->addWidget(label_3, 1, 0, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font);

        gridLayout_2->addWidget(label_2, 0, 2, 1, 1);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setFont(font);

        gridLayout_2->addWidget(label_4, 1, 2, 1, 1);

        baudRate = new QComboBox(groupBox);
        baudRate->addItem(QString());
        baudRate->addItem(QString());
        baudRate->addItem(QString());
        baudRate->addItem(QString());
        baudRate->addItem(QString());
        baudRate->addItem(QString());
        baudRate->setObjectName(QString::fromUtf8("baudRate"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(baudRate->sizePolicy().hasHeightForWidth());
        baudRate->setSizePolicy(sizePolicy);
        baudRate->setSizeIncrement(QSize(0, 0));
        baudRate->setFont(font);

        gridLayout_2->addWidget(baudRate, 0, 3, 1, 1);

        portSelect = new QComboBox(groupBox);
        portSelect->setObjectName(QString::fromUtf8("portSelect"));
        portSelect->setFont(font);
        portSelect->setMaxVisibleItems(10);

        gridLayout_2->addWidget(portSelect, 0, 1, 1, 1);

        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setFont(font);
        label->setTextFormat(Qt::AutoText);

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        stopBits = new QComboBox(groupBox);
        stopBits->addItem(QString());
        stopBits->addItem(QString());
        stopBits->addItem(QString());
        stopBits->setObjectName(QString::fromUtf8("stopBits"));
        stopBits->setFont(font);

        gridLayout_2->addWidget(stopBits, 1, 1, 1, 1);

        openPort = new QPushButton(groupBox);
        openPort->setObjectName(QString::fromUtf8("openPort"));
        openPort->setEnabled(true);

        gridLayout_2->addWidget(openPort, 1, 5, 1, 1);

        dataBits = new QComboBox(groupBox);
        dataBits->addItem(QString());
        dataBits->addItem(QString());
        dataBits->addItem(QString());
        dataBits->addItem(QString());
        dataBits->setObjectName(QString::fromUtf8("dataBits"));
        dataBits->setFont(font);

        gridLayout_2->addWidget(dataBits, 1, 3, 1, 1);

        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setFont(font);

        gridLayout_2->addWidget(label_6, 1, 4, 1, 1);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setFont(font);

        gridLayout_2->addWidget(label_5, 0, 4, 1, 1);

        parity = new QComboBox(groupBox);
        parity->addItem(QString());
        parity->addItem(QString());
        parity->addItem(QString());
        parity->addItem(QString());
        parity->addItem(QString());
        parity->setObjectName(QString::fromUtf8("parity"));
        parity->setFont(font);

        gridLayout_2->addWidget(parity, 0, 5, 1, 1);


        gridLayout_4->addWidget(groupBox, 0, 0, 1, 1);

        groupBox_3 = new QGroupBox(widget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setFont(font1);
        gridLayout = new QGridLayout(groupBox_3);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        connect_db = new QPushButton(groupBox_3);
        connect_db->setObjectName(QString::fromUtf8("connect_db"));

        gridLayout->addWidget(connect_db, 1, 2, 1, 1, Qt::AlignHCenter);

        label_8 = new QLabel(groupBox_3);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout->addWidget(label_8, 1, 0, 1, 1);

        start_collect = new QPushButton(groupBox_3);
        start_collect->setObjectName(QString::fromUtf8("start_collect"));
        start_collect->setFont(font);

        gridLayout->addWidget(start_collect, 0, 2, 1, 1);

        freq = new QLineEdit(groupBox_3);
        freq->setObjectName(QString::fromUtf8("freq"));

        gridLayout->addWidget(freq, 0, 1, 1, 1);

        label_7 = new QLabel(groupBox_3);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout->addWidget(label_7, 0, 0, 1, 1);

        freq_num = new QLineEdit(groupBox_3);
        freq_num->setObjectName(QString::fromUtf8("freq_num"));

        gridLayout->addWidget(freq_num, 1, 1, 1, 1);

        hanning = new QPushButton(groupBox_3);
        hanning->setObjectName(QString::fromUtf8("hanning"));

        gridLayout->addWidget(hanning, 0, 3, 1, 1);

        hamming = new QPushButton(groupBox_3);
        hamming->setObjectName(QString::fromUtf8("hamming"));

        gridLayout->addWidget(hamming, 0, 4, 1, 1);

        export_data = new QPushButton(groupBox_3);
        export_data->setObjectName(QString::fromUtf8("export_data"));

        gridLayout->addWidget(export_data, 1, 3, 1, 1);

        clear_db = new QPushButton(groupBox_3);
        clear_db->setObjectName(QString::fromUtf8("clear_db"));

        gridLayout->addWidget(clear_db, 1, 4, 1, 1);


        gridLayout_4->addWidget(groupBox_3, 0, 1, 1, 1);

        gridLayout_4->setColumnMinimumWidth(0, 1);

        verticalLayout->addWidget(widget);

        verticalLayout->setStretch(0, 10);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1027, 22));
        MainWindow->setMenuBar(menubar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "Display waveform", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "Serial Port", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "StopBits", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "BaudRate", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "DataBits", nullptr));
        baudRate->setItemText(0, QCoreApplication::translate("MainWindow", "115200", nullptr));
        baudRate->setItemText(1, QCoreApplication::translate("MainWindow", "1200", nullptr));
        baudRate->setItemText(2, QCoreApplication::translate("MainWindow", "2400", nullptr));
        baudRate->setItemText(3, QCoreApplication::translate("MainWindow", "4800", nullptr));
        baudRate->setItemText(4, QCoreApplication::translate("MainWindow", "9600", nullptr));
        baudRate->setItemText(5, QCoreApplication::translate("MainWindow", "19200", nullptr));

        baudRate->setCurrentText(QCoreApplication::translate("MainWindow", "115200", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "PortName", nullptr));
        stopBits->setItemText(0, QCoreApplication::translate("MainWindow", "1", nullptr));
        stopBits->setItemText(1, QCoreApplication::translate("MainWindow", "1.5", nullptr));
        stopBits->setItemText(2, QCoreApplication::translate("MainWindow", "2", nullptr));

        openPort->setText(QCoreApplication::translate("MainWindow", "openPort", nullptr));
        dataBits->setItemText(0, QCoreApplication::translate("MainWindow", "8", nullptr));
        dataBits->setItemText(1, QCoreApplication::translate("MainWindow", "5", nullptr));
        dataBits->setItemText(2, QCoreApplication::translate("MainWindow", "6", nullptr));
        dataBits->setItemText(3, QCoreApplication::translate("MainWindow", "7", nullptr));

        label_6->setText(QCoreApplication::translate("MainWindow", "Operation", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Parity", nullptr));
        parity->setItemText(0, QCoreApplication::translate("MainWindow", "None", nullptr));
        parity->setItemText(1, QCoreApplication::translate("MainWindow", "Even", nullptr));
        parity->setItemText(2, QCoreApplication::translate("MainWindow", "Odd", nullptr));
        parity->setItemText(3, QCoreApplication::translate("MainWindow", "Space", nullptr));
        parity->setItemText(4, QCoreApplication::translate("MainWindow", "Mark", nullptr));

        groupBox_3->setTitle(QCoreApplication::translate("MainWindow", "Analyses", nullptr));
        connect_db->setText(QCoreApplication::translate("MainWindow", "ConnDB", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Number", nullptr));
        start_collect->setText(QCoreApplication::translate("MainWindow", "Start", nullptr));
        freq->setText(QCoreApplication::translate("MainWindow", "1000", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Frequency", nullptr));
        freq_num->setText(QCoreApplication::translate("MainWindow", "1000", nullptr));
        hanning->setText(QCoreApplication::translate("MainWindow", "Hanning", nullptr));
        hamming->setText(QCoreApplication::translate("MainWindow", "Hamming", nullptr));
        export_data->setText(QCoreApplication::translate("MainWindow", "ExportDatas", nullptr));
        clear_db->setText(QCoreApplication::translate("MainWindow", "ClearDatas", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
