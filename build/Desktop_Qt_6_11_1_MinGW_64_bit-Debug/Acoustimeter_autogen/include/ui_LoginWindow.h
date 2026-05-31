/********************************************************************************
** Form generated from reading UI file 'LoginWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINWINDOW_H
#define UI_LOGINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginWindow
{
public:
    QVBoxLayout *mainLayout;
    QFrame *headerFrame;
    QHBoxLayout *headerLayout;
    QFrame *niBadge;
    QVBoxLayout *niBadgeLayout;
    QLabel *niLabel;
    QLabel *niModel;
    QVBoxLayout *titleLayout;
    QLabel *logoLabel;
    QLabel *subLabel;
    QWidget *bodyWidget;
    QVBoxLayout *bodyLayout;
    QLabel *secChassis;
    QHBoxLayout *chassisRow;
    QVBoxLayout *chassisCol;
    QLabel *lblChassis;
    QComboBox *comboChassis;
    QVBoxLayout *slotCol;
    QLabel *lblSlot;
    QSpinBox *spinSlot;
    QVBoxLayout *ifaceCol;
    QLabel *lblIface;
    QComboBox *comboInterface;
    QLabel *visaPlaceholder;
    QLabel *secProfile;
    QWidget *profileContainer;
    QWidget *pwdWidget;
    QVBoxLayout *pwdLayout;
    QLabel *lblPwd;
    QLineEdit *editPassword;
    QPushButton *btnConnect;
    QWidget *progressWidget;
    QVBoxLayout *progressLayout;
    QHBoxLayout *progressTopRow;
    QLabel *lblProgressMsg;
    QLabel *lblProgressPct;
    QProgressBar *progressBar;
    QListWidget *listSteps;
    QFrame *footerFrame;
    QHBoxLayout *footerLayout;
    QLabel *lblStatus;
    QLabel *lblVersion;

    void setupUi(QDialog *LoginWindow)
    {
        if (LoginWindow->objectName().isEmpty())
            LoginWindow->setObjectName("LoginWindow");
        LoginWindow->resize(440, 580);
        LoginWindow->setMinimumSize(QSize(440, 580));
        mainLayout = new QVBoxLayout(LoginWindow);
        mainLayout->setSpacing(0);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setObjectName("mainLayout");
        headerFrame = new QFrame(LoginWindow);
        headerFrame->setObjectName("headerFrame");
        headerFrame->setFrameShape(QFrame::StyledPanel);
        headerLayout = new QHBoxLayout(headerFrame);
        headerLayout->setSpacing(14);
        headerLayout->setObjectName("headerLayout");
        headerLayout->setContentsMargins(16, 12, 16, 12);
        niBadge = new QFrame(headerFrame);
        niBadge->setObjectName("niBadge");
        niBadge->setMinimumSize(QSize(60, 48));
        niBadgeLayout = new QVBoxLayout(niBadge);
        niBadgeLayout->setSpacing(2);
        niBadgeLayout->setObjectName("niBadgeLayout");
        niBadgeLayout->setContentsMargins(8, 4, 8, 4);
        niLabel = new QLabel(niBadge);
        niLabel->setObjectName("niLabel");
        niLabel->setAlignment(Qt::AlignCenter);

        niBadgeLayout->addWidget(niLabel);

        niModel = new QLabel(niBadge);
        niModel->setObjectName("niModel");
        niModel->setAlignment(Qt::AlignCenter);

        niBadgeLayout->addWidget(niModel);


        headerLayout->addWidget(niBadge);

        titleLayout = new QVBoxLayout();
        titleLayout->setSpacing(3);
        titleLayout->setObjectName("titleLayout");
        logoLabel = new QLabel(headerFrame);
        logoLabel->setObjectName("logoLabel");

        titleLayout->addWidget(logoLabel);

        subLabel = new QLabel(headerFrame);
        subLabel->setObjectName("subLabel");

        titleLayout->addWidget(subLabel);


        headerLayout->addLayout(titleLayout);


        mainLayout->addWidget(headerFrame);

        bodyWidget = new QWidget(LoginWindow);
        bodyWidget->setObjectName("bodyWidget");
        bodyLayout = new QVBoxLayout(bodyWidget);
        bodyLayout->setSpacing(10);
        bodyLayout->setObjectName("bodyLayout");
        bodyLayout->setContentsMargins(20, 14, 20, 14);
        secChassis = new QLabel(bodyWidget);
        secChassis->setObjectName("secChassis");

        bodyLayout->addWidget(secChassis);

        chassisRow = new QHBoxLayout();
        chassisRow->setSpacing(8);
        chassisRow->setObjectName("chassisRow");
        chassisCol = new QVBoxLayout();
        chassisCol->setSpacing(3);
        chassisCol->setObjectName("chassisCol");
        lblChassis = new QLabel(bodyWidget);
        lblChassis->setObjectName("lblChassis");

        chassisCol->addWidget(lblChassis);

        comboChassis = new QComboBox(bodyWidget);
        comboChassis->addItem(QString());
        comboChassis->addItem(QString());
        comboChassis->addItem(QString());
        comboChassis->setObjectName("comboChassis");

        chassisCol->addWidget(comboChassis);


        chassisRow->addLayout(chassisCol);

        slotCol = new QVBoxLayout();
        slotCol->setSpacing(3);
        slotCol->setObjectName("slotCol");
        lblSlot = new QLabel(bodyWidget);
        lblSlot->setObjectName("lblSlot");

        slotCol->addWidget(lblSlot);

        spinSlot = new QSpinBox(bodyWidget);
        spinSlot->setObjectName("spinSlot");
        spinSlot->setMinimum(1);
        spinSlot->setMaximum(18);
        spinSlot->setValue(1);

        slotCol->addWidget(spinSlot);


        chassisRow->addLayout(slotCol);

        ifaceCol = new QVBoxLayout();
        ifaceCol->setSpacing(3);
        ifaceCol->setObjectName("ifaceCol");
        lblIface = new QLabel(bodyWidget);
        lblIface->setObjectName("lblIface");

        ifaceCol->addWidget(lblIface);

        comboInterface = new QComboBox(bodyWidget);
        comboInterface->addItem(QString());
        comboInterface->addItem(QString());
        comboInterface->setObjectName("comboInterface");

        ifaceCol->addWidget(comboInterface);


        chassisRow->addLayout(ifaceCol);


        bodyLayout->addLayout(chassisRow);

        visaPlaceholder = new QLabel(bodyWidget);
        visaPlaceholder->setObjectName("visaPlaceholder");

        bodyLayout->addWidget(visaPlaceholder);

        secProfile = new QLabel(bodyWidget);
        secProfile->setObjectName("secProfile");

        bodyLayout->addWidget(secProfile);

        profileContainer = new QWidget(bodyWidget);
        profileContainer->setObjectName("profileContainer");
        profileContainer->setMinimumHeight(36);

        bodyLayout->addWidget(profileContainer);

        pwdWidget = new QWidget(bodyWidget);
        pwdWidget->setObjectName("pwdWidget");
        pwdWidget->setVisible(false);
        pwdLayout = new QVBoxLayout(pwdWidget);
        pwdLayout->setSpacing(3);
        pwdLayout->setObjectName("pwdLayout");
        pwdLayout->setContentsMargins(0, 0, 0, 0);
        lblPwd = new QLabel(pwdWidget);
        lblPwd->setObjectName("lblPwd");

        pwdLayout->addWidget(lblPwd);

        editPassword = new QLineEdit(pwdWidget);
        editPassword->setObjectName("editPassword");
        editPassword->setEchoMode(QLineEdit::Password);

        pwdLayout->addWidget(editPassword);


        bodyLayout->addWidget(pwdWidget);

        btnConnect = new QPushButton(bodyWidget);
        btnConnect->setObjectName("btnConnect");
        btnConnect->setMinimumHeight(36);

        bodyLayout->addWidget(btnConnect);

        progressWidget = new QWidget(bodyWidget);
        progressWidget->setObjectName("progressWidget");
        progressWidget->setVisible(false);
        progressLayout = new QVBoxLayout(progressWidget);
        progressLayout->setSpacing(4);
        progressLayout->setObjectName("progressLayout");
        progressLayout->setContentsMargins(0, 0, 0, 0);
        progressTopRow = new QHBoxLayout();
        progressTopRow->setObjectName("progressTopRow");
        lblProgressMsg = new QLabel(progressWidget);
        lblProgressMsg->setObjectName("lblProgressMsg");

        progressTopRow->addWidget(lblProgressMsg);

        lblProgressPct = new QLabel(progressWidget);
        lblProgressPct->setObjectName("lblProgressPct");
        lblProgressPct->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

        progressTopRow->addWidget(lblProgressPct);


        progressLayout->addLayout(progressTopRow);

        progressBar = new QProgressBar(progressWidget);
        progressBar->setObjectName("progressBar");
        progressBar->setValue(0);
        progressBar->setTextVisible(false);

        progressLayout->addWidget(progressBar);

        listSteps = new QListWidget(progressWidget);
        listSteps->setObjectName("listSteps");
        listSteps->setMaximumHeight(90);
        listSteps->setFrameShape(QFrame::NoFrame);

        progressLayout->addWidget(listSteps);


        bodyLayout->addWidget(progressWidget);


        mainLayout->addWidget(bodyWidget);

        footerFrame = new QFrame(LoginWindow);
        footerFrame->setObjectName("footerFrame");
        footerLayout = new QHBoxLayout(footerFrame);
        footerLayout->setObjectName("footerLayout");
        footerLayout->setContentsMargins(16, 6, 16, 6);
        lblStatus = new QLabel(footerFrame);
        lblStatus->setObjectName("lblStatus");

        footerLayout->addWidget(lblStatus);

        lblVersion = new QLabel(footerFrame);
        lblVersion->setObjectName("lblVersion");
        lblVersion->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

        footerLayout->addWidget(lblVersion);


        mainLayout->addWidget(footerFrame);


        retranslateUi(LoginWindow);

        QMetaObject::connectSlotsByName(LoginWindow);
    } // setupUi

    void retranslateUi(QDialog *LoginWindow)
    {
        LoginWindow->setWindowTitle(QCoreApplication::translate("LoginWindow", "ACOUSTIMETER \342\200\224 Connexion", nullptr));
        niLabel->setText(QCoreApplication::translate("LoginWindow", "NI", nullptr));
        niModel->setText(QCoreApplication::translate("LoginWindow", "PXIe-8301", nullptr));
        logoLabel->setText(QCoreApplication::translate("LoginWindow", "ACOUSTIMETER", nullptr));
        subLabel->setText(QCoreApplication::translate("LoginWindow", "Logiciel de pilotage et mesure acoustique \342\200\224 v1.0", nullptr));
        secChassis->setText(QCoreApplication::translate("LoginWindow", "Identification ch\303\242ssis PXI Express", nullptr));
        lblChassis->setText(QCoreApplication::translate("LoginWindow", "Ch\303\242ssis", nullptr));
        comboChassis->setItemText(0, QCoreApplication::translate("LoginWindow", "PXI0", nullptr));
        comboChassis->setItemText(1, QCoreApplication::translate("LoginWindow", "PXI1", nullptr));
        comboChassis->setItemText(2, QCoreApplication::translate("LoginWindow", "PXI2", nullptr));

        lblSlot->setText(QCoreApplication::translate("LoginWindow", "Slot PXIe-8301", nullptr));
        lblIface->setText(QCoreApplication::translate("LoginWindow", "Interface h\303\264te", nullptr));
        comboInterface->setItemText(0, QCoreApplication::translate("LoginWindow", "Thunderbolt 3", nullptr));
        comboInterface->setItemText(1, QCoreApplication::translate("LoginWindow", "MXI-Express", nullptr));

        visaPlaceholder->setText(QCoreApplication::translate("LoginWindow", "PXI0::CHASSIS1::SLOT1::INSTR", nullptr));
        secProfile->setText(QCoreApplication::translate("LoginWindow", "Profil utilisateur", nullptr));
        lblPwd->setText(QCoreApplication::translate("LoginWindow", "Mot de passe administrateur", nullptr));
        editPassword->setPlaceholderText(QCoreApplication::translate("LoginWindow", "\342\200\242\342\200\242\342\200\242\342\200\242\342\200\242\342\200\242\342\200\242\342\200\242", nullptr));
        btnConnect->setText(QCoreApplication::translate("LoginWindow", "\342\226\266  Connecter au ch\303\242ssis", nullptr));
        lblProgressMsg->setText(QCoreApplication::translate("LoginWindow", "Initialisation...", nullptr));
        lblProgressPct->setText(QCoreApplication::translate("LoginWindow", "0%", nullptr));
        lblStatus->setText(QCoreApplication::translate("LoginWindow", "En attente de connexion", nullptr));
        lblVersion->setText(QCoreApplication::translate("LoginWindow", "NI PXIe-8301 \302\267 Thunderbolt 3 Remote Controller", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginWindow: public Ui_LoginWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINWINDOW_H
