#pragma once
#include <QDialog>
#include <QTimer>
#include "DataTypes.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE

class QProgressBar;
class QLabel;
class QListWidget;
class QPushButton;
class QComboBox;
class QLineEdit;

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget* parent = nullptr);
    ~LoginWindow();

    ConnectionConfig connectionConfig() const { return m_config; }

signals:
    void connectionEstablished(const ConnectionConfig& config);

private slots:
    void onProfileToggled();
    void onConnectClicked();
    void onChassisChanged();
    void advanceProgress();

private:
    void setupUi();
    void setupConnections();
    void updateVisaString();
    void startConnectionSequence();
    void onConnectionDone();

    Ui::LoginWindow* ui = nullptr;

    // Widgets créés en code (complément du .ui)
    QLabel*       m_visaLabel    = nullptr;
    QListWidget*  m_logList      = nullptr;
    QProgressBar* m_progressBar  = nullptr;
    QLabel*       m_progressMsg  = nullptr;
    QPushButton*  m_btnOp        = nullptr;
    QPushButton*  m_btnAdmin     = nullptr;

    QTimer*  m_timer = nullptr;
    int      m_stepIndex = 0;

    ConnectionConfig m_config;

    static const QStringList CONNECTION_STEPS;
};
