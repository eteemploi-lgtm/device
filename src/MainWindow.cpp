#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "GraphWidget.h"
#include "ConfigWindow.h"
#include "StyleManager.h"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QTimer>
#include <QTextEdit>
#include <QToolBar>
#include <QDateTime>
#include <QRandomGenerator>
#include <QApplication>
#include <QDebug>

const QList<QPair<QString,QString>> MainWindow::LOG_MESSAGES = {
    {"ok",   "Système initialisé — 4 hydrophones, 2 amplificateurs"},
    {"ok",   "AMP1 actif — MLI 38.4 kHz générée"},
    {"warn", "Hydro 3 : niveau de bruit élevé (+6 dB)"},
    {"ok",   "Acquisition démarrée — Fs 96 kHz"},
    {"err",  "Hydro 4 : perte de signal — vérifier connectique"},
    {"warn", "T2 : température 47.8°C (seuil 50°C)"},
    {"ok",   "Config. sauvegardée → config_2024.json"},
    {"ok",   "Sv démarrée — gîte 0°→360°, résolution 1°"},
    {"warn", "I_AMP1 : courant élevé 3.8A (max 5A)"},
    };

MainWindow::MainWindow(const ConnectionConfig& cfg, QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_connConfig(cfg)
{
    ui->setupUi(this);

    for (auto& k : {"h1","h2","h3","h4","a1","a2"})
        m_voies[k] = true;

    setStyleSheet(StyleManager::instance().mainStyleSheet());

    setupMenuBar();
    setupVoieBar();
    setupCharts();
    setupSidebar();
    setupLogView();
    setupStatusBar();

    m_metricTimer = new QTimer(this);
    connect(m_metricTimer, &QTimer::timeout, this, &MainWindow::updateMetrics);
    m_metricTimer->start(1200);

    m_logTimer = new QTimer(this);
    connect(m_logTimer, &QTimer::timeout, this, [this](){
        auto& msg = LOG_MESSAGES[m_logMsgIdx % LOG_MESSAGES.size()];
        appendLog(msg.first, msg.second);
        m_logMsgIdx++;
    });
    m_logTimer->start(3500);
    appendLog("ok", "Connexion établie — " + cfg.chassis + " · NI PXIe-8301");
}

MainWindow::~MainWindow()
{
    delete m_configWindow;
    delete ui;
}

// ─── Menu bar ────────────────────────────────────────────────────────────────
void MainWindow::setupMenuBar()
{
    auto* mFile = menuBar()->addMenu("Fichier");
    mFile->addAction("Nouvelle session");
    mFile->addAction("Ouvrir session...");
    mFile->addSeparator();
    mFile->addAction("Sauvegarder");
    mFile->addAction("Exporter données...");
    mFile->addSeparator();
    auto* aQuit = mFile->addAction("Quitter");
    connect(aQuit, &QAction::triggered, qApp, &QApplication::quit);

    auto* mAcq = menuBar()->addMenu("Acquisition");
    mAcq->addAction("Démarrer acquisition");
    mAcq->addAction("Arrêter acquisition");
    mAcq->addSeparator();
    mAcq->addAction("Démarrer mesure Sv");

    auto* mTools = menuBar()->addMenu("Outils");
    mTools->addAction("Calibration");
    mTools->addAction("Diagnostic matériel");
    auto* aCfg = mTools->addAction("Configuration...");
    connect(aCfg, &QAction::triggered, this, &MainWindow::openConfig);

    auto* mHelp = menuBar()->addMenu("Aide");
    auto* aTheme = mHelp->addAction("◐  Thème clair / sombre");
    connect(aTheme, &QAction::triggered, this, &MainWindow::toggleTheme);
    mHelp->addSeparator();
    mHelp->addAction("Documentation");
    mHelp->addAction("À propos...");

    // Toolbar
    auto* tb = addToolBar("main");
    tb->setMovable(false);

    auto* logoLbl = new QLabel("  ACOUSTIMETER  ");
    logoLbl->setStyleSheet(
        "color:#00d4ff; font-size:16px; font-weight:bold; letter-spacing:2px;");
    tb->addWidget(logoLbl);
    tb->addSeparator();

    auto* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    tb->addWidget(spacer);

    auto* themeBtn = new QPushButton("◐");
    themeBtn->setObjectName("themeBtn");
    themeBtn->setFixedWidth(34);
    connect(themeBtn, &QPushButton::clicked, this, &MainWindow::toggleTheme);
    tb->addWidget(themeBtn);
    tb->addSeparator();

    QString profStr = (m_connConfig.profile == UserProfile::Administrator)
                          ? "🔑 Admin" : "👤 Opérateur";
    auto* userLabel = new QLabel(
        QString("  %1  ·  <span style='color:#00d4ff'>%2</span>  ")
            .arg(profStr, m_connConfig.chassis));
    userLabel->setTextFormat(Qt::RichText);
    userLabel->setStyleSheet("color:#8b949e; font-size:13px;");
    tb->addWidget(userLabel);

    auto* cfgBtn = new QPushButton("⚙  CONFIG");
    cfgBtn->setProperty("cssClass","connect");
    cfgBtn->setFixedHeight(30);
    cfgBtn->setMinimumWidth(110);
    connect(cfgBtn, &QPushButton::clicked, this, &MainWindow::openConfig);
    tb->addWidget(cfgBtn);
    tb->addSeparator();
}

// ─── Barre des voies ─────────────────────────────────────────────────────────
void MainWindow::setupVoieBar()
{
    auto* layout = qobject_cast<QHBoxLayout*>(ui->voieBar->layout());

    struct VoieDef { QString key; QString label; QString color; };
    QList<VoieDef> hydros = {
        {"h1","H1","#00d4ff"}, {"h2","H2","#00e676"},
        {"h3","H3","#ffaa00"}, {"h4","H4","#ff6b9d"}
    };
    QList<VoieDef> amplis = {
        {"a1","A1","#c792ea"}, {"a2","A2","#89ddff"}
    };

    auto addLabel = [&](const QString& txt){
        auto* l = new QLabel(txt);
        l->setStyleSheet("color:#6e7681; font-size:13px; font-weight:bold;");
        layout->addWidget(l);
    };

    auto addBtn = [&](const VoieDef& d) {
        auto* btn = new QPushButton(d.label);
        btn->setCheckable(true);
        btn->setChecked(true);
        btn->setFixedHeight(26);
        btn->setMinimumWidth(44);
        btn->setStyleSheet(QString(
                               "QPushButton { border:1px solid %1; color:%1; background:transparent;"
                               "  font-size:13px; font-weight:bold; border-radius:2px; padding:2px 8px; }"
                               "QPushButton:checked { background:%2; }"
                               "QPushButton:!checked { opacity:0.4; color:#444c56; border-color:#444c56; }"
                               ).arg(d.color, d.color + "22"));
        btn->setProperty("voieKey", d.key);
        connect(btn, &QPushButton::toggled, this, &MainWindow::onToggleVoie);
        layout->addWidget(btn);
        m_voieBtns[d.key] = btn;
    };

    auto addSep = [&](){
        auto* s = new QFrame();
        s->setFrameShape(QFrame::VLine);
        s->setStyleSheet("color:#30363d;");
        layout->addWidget(s);
    };

    addLabel("HYDROS :");
    for (auto& d : hydros) addBtn(d);
    addSep();
    addLabel("AMPLIS :");
    for (auto& d : amplis) addBtn(d);
    addSep();

    auto* infoLabel = new QLabel(
        "Fs  <span style='color:#00d4ff;font-weight:bold'>96 kHz</span>"
        "  &nbsp;|&nbsp;  MLI  "
        "<span style='color:#00e676;font-weight:bold'>38.4 kHz</span>");
    infoLabel->setTextFormat(Qt::RichText);
    infoLabel->setStyleSheet("font-size:13px; color:#8b949e;");
    layout->addWidget(infoLabel);
    layout->addStretch();
}

// ─── Graphiques ──────────────────────────────────────────────────────────────
void MainWindow::setupCharts()
{
    auto* grid = qobject_cast<QGridLayout*>(ui->chartsContainer->layout());

    auto makeBox = [](const QString& title, GraphWidget* gw) {
        auto* box = new QGroupBox(title);
        auto* l = new QVBoxLayout(box);
        l->setContentsMargins(4,18,4,4);
        l->addWidget(gw);
        return box;
    };

    m_graphTime   = new GraphWidget(GraphWidget::TimeDomain, this);
    m_graphFFT    = new GraphWidget(GraphWidget::FFT, this);
    m_graphLevels = new GraphWidget(GraphWidget::Levels, this);

    grid->addWidget(makeBox("Signal temporel — hydrophones", m_graphTime),   0, 0, 1, 2);
    grid->addWidget(makeBox("FFT",                           m_graphFFT),    1, 0);
    grid->addWidget(makeBox("Niveaux — dB re 1µPa",         m_graphLevels), 1, 1);

    grid->setRowStretch(0, 1);
    grid->setRowStretch(1, 1);
}

// ─── Sidebar ─────────────────────────────────────────────────────────────────
void MainWindow::setupSidebar()
{
    auto* layout = qobject_cast<QVBoxLayout*>(ui->sidebar->layout());
    ui->sidebar->setStyleSheet(
        "background:#161b22; border-left:1px solid #30363d;");

    struct Row { QString label; QString value; QString state; };
    auto addSection = [&](const QString& title, QList<Row> rows) {
        auto* box = new QGroupBox(title);
        auto* l = new QVBoxLayout(box);
        l->setSpacing(4);
        l->setContentsMargins(8,18,8,8);
        for (auto& row : rows) {
            auto* hl = new QHBoxLayout();
            auto* lbl = new QLabel(row.label);
            lbl->setObjectName("metricLabel");
            lbl->setStyleSheet("font-size:13px; color:#8b949e;");
            auto* val = new QLabel(row.value);
            val->setObjectName("metricValue");
            val->setStyleSheet(
                row.state == "warn" ? "font-weight:bold;font-size:14px;color:#ffaa00;" :
                    row.state == "err"  ? "font-weight:bold;font-size:14px;color:#ff4444;" :
                    row.state == "info" ? "font-weight:bold;font-size:14px;color:#00d4ff;" :
                    "font-weight:bold;font-size:14px;color:#00e676;");
            val->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            hl->addWidget(lbl);
            hl->addWidget(val);
            l->addLayout(hl);
        }
        layout->addWidget(box);
    };

    addSection("Amplificateurs", {
                                     {"AMP 1", "ACTIF",   "ok"},
                                     {"AMP 2", "STANDBY", "warn"}
                                 });
    addSection("Hydrophones", {
                                  {"Hydro 1", "ACQ",    "ok"},
                                  {"Hydro 2", "ACQ",    "ok"},
                                  {"Hydro 3", "BRUIT↑", "warn"},
                                  {"Hydro 4", "DÉCO",   "err"}
                              });
    addSection("Températures", {
                                   {"T1 (K)", "24.3°C", "ok"},
                                   {"T2 (J)", "47.8°C", "warn"},
                                   {"T3 (K)", "21.1°C", "ok"}
                               });
    addSection("Tensions", {
                               {"V Bus 48V",   "48.2V", "ok"},
                               {"V Alim 12V",  "12.1V", "ok"}
                           });
    addSection("Courants", {
                               {"I AMP1", "3.8A", "warn"},
                               {"I AMP2", "0.4A", "ok"}
                           });
    addSection("Mesure Sv", {
                                {"Gîte",        "127.5°",   "info"},
                                {"Site",        "32.0°",    "info"},
                                {"Sv moyen",    "−18.4 dB", "info"},
                                {"Progression", "35%",      "info"}
                            });

    layout->addStretch();
}

// ─── Log ─────────────────────────────────────────────────────────────────────
void MainWindow::setupLogView()
{
    ui->logView->setReadOnly(true);
    ui->logView->document()->setMaximumBlockCount(200);
}

void MainWindow::appendLog(const QString& level, const QString& msg)
{
    QString ts = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString color = (level == "ok")   ? "#00e676"
                    : (level == "warn") ? "#ffaa00" : "#ff4444";
    ui->logView->append(
        QString("<span style='color:#6e7681;font-size:12px'>%1</span>"
                "&nbsp;&nbsp;<span style='color:%2;font-size:13px'>%3</span>")
            .arg(ts, color, msg));
}

// ─── StatusBar ───────────────────────────────────────────────────────────────
void MainWindow::setupStatusBar()
{
    statusBar()->showMessage(
        QString("Connecté — %1 · NI PXIe-8301 · Fs 96 kHz")
            .arg(m_connConfig.chassis));
}

// ─── Toggle voie ─────────────────────────────────────────────────────────────
void MainWindow::onToggleVoie()
{
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    QString key = btn->property("voieKey").toString();
    bool on = btn->isChecked();
    m_voies[key] = on;
    if (m_graphTime)   m_graphTime->setVoieActive(key, on);
    if (m_graphLevels) m_graphLevels->setVoieActive(key, on);
}

// ─── Métriques ───────────────────────────────────────────────────────────────
void MainWindow::updateMetrics()
{
    (void)QRandomGenerator::global()->generate();
}

// ─── Thème ───────────────────────────────────────────────────────────────────
void MainWindow::toggleTheme()
{
    auto& sm = StyleManager::instance();
    sm.setTheme(sm.isDark() ? StyleManager::Light : StyleManager::Dark);
    setStyleSheet(sm.mainStyleSheet());
    if (m_configWindow)
        m_configWindow->setStyleSheet(sm.configStyleSheet());
    update();
}

// ─── Config ──────────────────────────────────────────────────────────────────
void MainWindow::openConfig()
{
    try {
        if (!m_configWindow) {
            m_configWindow = new ConfigWindow(nullptr);
            m_configWindow->setStyleSheet(
                StyleManager::instance().configStyleSheet());
            m_configWindow->setAttribute(Qt::WA_DeleteOnClose, false);
            m_configWindow->resize(1100, 750);
            m_configWindow->move(
                this->x() + 60,
                this->y() + 40);
        }
        m_configWindow->show();
        m_configWindow->raise();
        m_configWindow->activateWindow();
    } catch (...) {
        qDebug() << "Erreur ouverture ConfigWindow";
    }
}
