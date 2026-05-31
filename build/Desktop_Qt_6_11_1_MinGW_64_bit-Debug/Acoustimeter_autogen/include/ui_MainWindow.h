/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *mainVLayout;
    QWidget *voieBar;
    QHBoxLayout *voieBarLayout;
    QHBoxLayout *centerLayout;
    QWidget *chartsContainer;
    QGridLayout *chartsGrid;
    QWidget *sidebar;
    QVBoxLayout *sidebarLayout;
    QTextEdit *logView;
    QMenuBar *menuBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1280, 800);
        MainWindow->setMinimumSize(QSize(900, 600));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName("centralWidget");
        mainVLayout = new QVBoxLayout(centralWidget);
        mainVLayout->setSpacing(0);
        mainVLayout->setObjectName("mainVLayout");
        mainVLayout->setContentsMargins(0, 0, 0, 0);
        voieBar = new QWidget(centralWidget);
        voieBar->setObjectName("voieBar");
        voieBar->setMinimumHeight(32);
        voieBar->setMaximumHeight(32);
        voieBarLayout = new QHBoxLayout(voieBar);
        voieBarLayout->setSpacing(6);
        voieBarLayout->setObjectName("voieBarLayout");
        voieBarLayout->setContentsMargins(8, 0, 8, 0);

        mainVLayout->addWidget(voieBar);

        centerLayout = new QHBoxLayout();
        centerLayout->setSpacing(0);
        centerLayout->setObjectName("centerLayout");
        chartsContainer = new QWidget(centralWidget);
        chartsContainer->setObjectName("chartsContainer");
        chartsGrid = new QGridLayout(chartsContainer);
        chartsGrid->setSpacing(5);
        chartsGrid->setObjectName("chartsGrid");
        chartsGrid->setContentsMargins(6, 6, 6, 6);

        centerLayout->addWidget(chartsContainer);

        sidebar = new QWidget(centralWidget);
        sidebar->setObjectName("sidebar");
        sidebar->setMinimumWidth(200);
        sidebar->setMaximumWidth(210);
        sidebarLayout = new QVBoxLayout(sidebar);
        sidebarLayout->setSpacing(0);
        sidebarLayout->setObjectName("sidebarLayout");
        sidebarLayout->setContentsMargins(0, 0, 0, 0);

        centerLayout->addWidget(sidebar);


        mainVLayout->addLayout(centerLayout);

        logView = new QTextEdit(centralWidget);
        logView->setObjectName("logView");
        logView->setReadOnly(true);
        logView->setMaximumHeight(80);
        logView->setMinimumHeight(72);

        mainVLayout->addWidget(logView);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName("menuBar");
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName("statusBar");
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "ACOUSTIMETER \342\200\224 Supervision", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
