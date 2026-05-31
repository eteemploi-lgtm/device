#include "LoginWindow.h"
#include "ui_LoginWindow.h"
#include "StyleManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidgetItem>
#include <QTimer>
#include <QScreen>
#include <QApplication>

// ─── Étapes de connexion simulées (réalistes PXIe) ──────────────────────────
const QStringList LoginWindow::CONNECTION_STEPS = {
    "Énumération bus PXI Express...",
    "Détection NI PXIe-8301...",
    "Négociation lien Thunderbolt 3 (Gen3 x4)...",
    "Chargement firmware NI-VISA...",
    "Init NI-DAQmx — test ressources...",
    "Énumération modules dans le châssis...",
    "Self-calibration DAQmx...",
    "Vérification intégrité des voies...",
    "Châssis prêt — lien établi"
};

// ────────────────────────────────────────────────────────────────────────────

LoginWindow::LoginWindow(QWidget* parent)
    : QDialog(parent), ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setStyleSheet(StyleManager::instance().loginStyleSheet());
    setupUi();
    setupConnections();
    updateVisaString();
}

LoginWindow::~LoginWindow() { delete ui; }

// ─── Compléments UI en code ──────────────────────────────────────────────────
void LoginWindow::setupUi()
{
    // Boutons profil dans le conteneur prévu dans le .ui
    auto* profLayout = new QHBoxLayout(ui->profileContainer);
    profLayout->setSpacing(8);
    profLayout->setContentsMargins(0, 0, 0, 0);

    m_btnOp    = new QPushButton("👤  Opérateur",     ui->profileContainer);
    m_btnAdmin = new QPushButton("🔑  Administrateur", ui->profileContainer);

    m_btnOp->setCheckable(true);
    m_btnAdmin->setCheckable(true);
    m_btnOp->setChecked(true);
    m_btnOp->setProperty("cssClass", "primary");
    m_btnAdmin->setProperty("cssClass", "QPushButton");

    m_btnOp->setMinimumHeight(32);
    m_btnAdmin->setMinimumHeight(32);

    profLayout->addWidget(m_btnOp);
    profLayout->addWidget(m_btnAdmin);

    // Style initial
    m_btnOp->setStyleSheet("border-color:#00d4ff;color:#00d4ff;background:#001a2e;");
    m_btnAdmin->setStyleSheet("");

    // Centrer la fenêtre
    if (auto* screen = QApplication::primaryScreen()) {
        auto sg = screen->geometry();
        move(sg.center() - rect().center());
    }
}

void LoginWindow::setupConnections()
{
    connect(ui->comboChassis,   &QComboBox::currentIndexChanged, this, &LoginWindow::onChassisChanged);
    connect(ui->spinSlot,       &QSpinBox::valueChanged,         this, &LoginWindow::updateVisaString);
    connect(ui->comboInterface, &QComboBox::currentIndexChanged, this, [this](int){ updateVisaString(); });
    connect(ui->btnConnect,     &QPushButton::clicked,           this, &LoginWindow::onConnectClicked);
    connect(m_btnOp,            &QPushButton::clicked,           this, &LoginWindow::onProfileToggled);
    connect(m_btnAdmin,         &QPushButton::clicked,           this, &LoginWindow::onProfileToggled);
}

void LoginWindow::onChassisChanged() { updateVisaString(); }

// ─── VISA string  (commenté car pas de matériel) ────────────────────────────
void LoginWindow::updateVisaString()
{
    //  Format NI-VISA standard : "PXI0::CHASSIS1::SLOT1::INSTR"
    //  En production, cette chaîne est construite depuis niSys ou NI-MAX.
    //
    //  QString visa = QString("%1::CHASSIS1::SLOT%2::INSTR")
    //      .arg(ui->comboChassis->currentText())
    //      .arg(ui->spinSlot->value());
    //
    //  Simulation :
    QString visa = QString("%1::CHASSIS1::SLOT%2::INSTR")
        .arg(ui->comboChassis->currentText())
        .arg(ui->spinSlot->value());
    ui->visaPlaceholder->setText(visa);
}

void LoginWindow::onProfileToggled()
{
    bool adminSelected = (sender() == m_btnAdmin);
    m_btnOp->setChecked(!adminSelected);
    m_btnAdmin->setChecked(adminSelected);

    m_btnOp->setStyleSheet(
        adminSelected ? "" : "border-color:#00d4ff;color:#00d4ff;background:#001a2e;");
    m_btnAdmin->setStyleSheet(
        adminSelected ? "border-color:#c792ea;color:#c792ea;background:#1a0a2e;" : "");

    ui->pwdWidget->setVisible(adminSelected);
    adjustSize();
}

// ─── Séquence de connexion simulée ──────────────────────────────────────────
void LoginWindow::onConnectClicked()
{
    ui->btnConnect->setEnabled(false);
    ui->progressWidget->setVisible(true);
    ui->listSteps->clear();
    ui->progressBar->setValue(0);
    m_stepIndex = 0;

    ui->lblStatus->setText("Connexion en cours...");
    ui->lblStatus->setStyleSheet("color:#ffaa00;");

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &LoginWindow::advanceProgress);
    m_timer->start(380);
}

void LoginWindow::advanceProgress()
{
    if (m_stepIndex >= CONNECTION_STEPS.size()) {
        m_timer->stop();
        onConnectionDone();
        return;
    }

    int pct = (m_stepIndex + 1) * 100 / CONNECTION_STEPS.size();
    ui->progressBar->setValue(pct);
    ui->lblProgressPct->setText(QString("%1%").arg(pct));
    ui->lblProgressMsg->setText(CONNECTION_STEPS[m_stepIndex]);

    auto* item = new QListWidgetItem("✓  " + CONNECTION_STEPS[m_stepIndex]);
    item->setForeground(QColor("#00e676"));
    ui->listSteps->addItem(item);
    ui->listSteps->scrollToBottom();

    m_stepIndex++;
}

void LoginWindow::onConnectionDone()
{
    // Remplir la config
    m_config.chassis   = ui->comboChassis->currentText();
    m_config.slot      = ui->spinSlot->value();
    m_config.interface_= ui->comboInterface->currentText();
    m_config.profile   = m_btnAdmin->isChecked()
                         ? UserProfile::Administrator
                         : UserProfile::Operator;

    ui->lblStatus->setText(
        QString("Connecté — %1 · NI PXIe-8301").arg(m_config.chassis));
    ui->lblStatus->setStyleSheet("color:#00e676;");

    ui->btnConnect->setEnabled(true);
    ui->btnConnect->setText("▶  Lancer ACOUSTIMETER");

    disconnect(ui->btnConnect, &QPushButton::clicked, this, &LoginWindow::onConnectClicked);
    connect(ui->btnConnect, &QPushButton::clicked, this, [this](){
        emit connectionEstablished(m_config);
        accept();
    });
}
