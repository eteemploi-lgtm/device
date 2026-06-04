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
#include <QDialog>
#include <QDateTime>
#include <QRandomGenerator>
#include <QApplication>
#include <QDebug>
#include <QtMath>

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
    for (auto& k : {"h1","h2","h3","h4","a1","a2"}) m_voies[k] = true;
    setStyleSheet(StyleManager::instance().mainStyleSheet());
    setupMenuBar();
    setupVoieBar();
    setupCharts();
    setupSidebar();
    setupLogView();
    setupStatusBar();
    m_metricTimer = new QTimer(this);
    connect(m_metricTimer, &QTimer::timeout, this, &MainWindow::updateMetrics);
    m_metricTimer->start(400);
    m_logTimer = new QTimer(this);
    connect(m_logTimer, &QTimer::timeout, this, [this](){
        auto& msg = LOG_MESSAGES[m_logMsgIdx % LOG_MESSAGES.size()];
        appendLog(msg.first, msg.second); m_logMsgIdx++;
    });
    m_logTimer->start(3500);
    appendLog("ok", "Connexion établie — " + cfg.chassis + " · NI PXIe-8301");
}

MainWindow::~MainWindow() {
    if (m_configWindow) { m_configWindow->close(); m_configWindow = nullptr; }
    delete ui;
}

void MainWindow::showAbout()
{
    QDialog* dlg = new QDialog(this);
    dlg->setWindowTitle("À propos — ACOUSTIMETER");
    dlg->setFixedSize(380, 240);
    dlg->setStyleSheet(StyleManager::instance().appStyleSheet());
    auto* l = new QVBoxLayout(dlg);
    l->setSpacing(10); l->setContentsMargins(24,20,24,20);
    auto* logo = new QLabel("ACOUSTIMETER");
    logo->setObjectName("logoLabel"); logo->setAlignment(Qt::AlignCenter);
    l->addWidget(logo);
    auto* sub = new QLabel("Logiciel de pilotage et mesure acoustique");
    sub->setAlignment(Qt::AlignCenter); sub->setObjectName("subLabel");
    l->addWidget(sub);
    auto addRow = [&](const QString& lbl, const QString& val){
        auto* hl = new QHBoxLayout();
        auto* lb = new QLabel(lbl); lb->setStyleSheet("color:#8b949e;font-size:13px;");
        auto* vl = new QLabel(val); vl->setStyleSheet("color:#00d4ff;font-size:13px;font-weight:bold;");
        vl->setAlignment(Qt::AlignRight);
        hl->addWidget(lb); hl->addWidget(vl); l->addLayout(hl);
    };
    addRow("Version",   "1.0");
    addRow("Matériel",  "NI PXIe-8301");
    addRow("Interface", m_connConfig.interface_);
    addRow("Châssis",   m_connConfig.chassis);
    addRow("Qt",        QT_VERSION_STR);
    auto* ok = new QPushButton("Fermer");
    ok->setProperty("cssClass","primary");
    connect(ok, &QPushButton::clicked, dlg, &QDialog::accept);
    l->addWidget(ok);
    dlg->exec();
}

void MainWindow::setupMenuBar()
{
    auto* mFile = menuBar()->addMenu("Fichier");
    mFile->addAction("Nouvelle session");
    mFile->addAction("Ouvrir session...");
    mFile->addSeparator();
    mFile->addAction("Sauvegarder");
    mFile->addAction("Exporter données...");
    mFile->addSeparator();
    connect(mFile->addAction("Quitter"), &QAction::triggered, qApp, &QApplication::quit);

    auto* mAcq = menuBar()->addMenu("Acquisition");
    mAcq->addAction("Démarrer acquisition");
    mAcq->addAction("Arrêter acquisition");
    mAcq->addSeparator();
    mAcq->addAction("Démarrer mesure Sv");

    auto* mTools = menuBar()->addMenu("Outils");
    mTools->addAction("Calibration");
    mTools->addAction("Diagnostic matériel");
    connect(mTools->addAction("Configuration..."), &QAction::triggered,
            this, &MainWindow::openConfig);

    auto* mHelp = menuBar()->addMenu("Aide");
    connect(mHelp->addAction("◐  Thème clair / sombre"), &QAction::triggered,
            this, &MainWindow::toggleTheme);
    mHelp->addSeparator();
    mHelp->addAction("Documentation");
    connect(mHelp->addAction("À propos..."), &QAction::triggered,
            this, &MainWindow::showAbout);

    auto* tb = addToolBar("main");
    tb->setMovable(false);
    auto* logoLbl = new QLabel("  ACOUSTIMETER  ");
    logoLbl->setStyleSheet("color:#00d4ff;font-size:16px;font-weight:bold;letter-spacing:2px;");
    tb->addWidget(logoLbl);
    tb->addSeparator();

    m_ledLabel = new QLabel("●");
    m_ledLabel->setStyleSheet("color:#444c56;font-size:16px;");
    m_ledLabel->setFixedWidth(22);
    tb->addWidget(m_ledLabel);

    m_acqButton = new QPushButton("▶  DÉMARRER ÉMISSION");
    m_acqButton->setProperty("cssClass","connect");
    m_acqButton->setFixedHeight(30);
    m_acqButton->setMinimumWidth(190);
    connect(m_acqButton, &QPushButton::clicked, this, &MainWindow::toggleAcquisition);
    tb->addWidget(m_acqButton);
    tb->addSeparator();

    auto* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    tb->addWidget(spacer);

    auto* themeBtn = new QPushButton("◐");
    themeBtn->setObjectName("themeBtn"); themeBtn->setFixedWidth(34);
    connect(themeBtn, &QPushButton::clicked, this, &MainWindow::toggleTheme);
    tb->addWidget(themeBtn);
    tb->addSeparator();

    QString profStr = (m_connConfig.profile == UserProfile::Administrator)
                      ? "🔑 Admin" : "👤 Opérateur";
    auto* userLabel = new QLabel(
        QString("  %1  ·  <span style='color:#00d4ff'>%2</span>  ")
        .arg(profStr, m_connConfig.chassis));
    userLabel->setTextFormat(Qt::RichText);
    userLabel->setStyleSheet("color:#8b949e;font-size:13px;");
    tb->addWidget(userLabel);

    auto* cfgBtn = new QPushButton("⚙  CONFIG");
    cfgBtn->setProperty("cssClass","primary");
    cfgBtn->setFixedHeight(30); cfgBtn->setMinimumWidth(100);
    connect(cfgBtn, &QPushButton::clicked, this, &MainWindow::openConfig);
    tb->addWidget(cfgBtn);
    tb->addSeparator();

    m_ledTimer = new QTimer(this);
    connect(m_ledTimer, &QTimer::timeout, this, &MainWindow::blinkLed);
}

void MainWindow::toggleAcquisition()
{
    m_acquiring = !m_acquiring;
    if (m_acquiring) {
        m_acqButton->setText("■  ARRÊTER ÉMISSION");
        m_acqButton->setStyleSheet(
            "background:#1a0000;border:1px solid #ff4444;color:#ff4444;"
            "font-size:14px;font-weight:bold;min-height:30px;border-radius:2px;");
        m_ledTimer->start(500);
        appendLog("ok","Émission acoustique démarrée — MLI 38.4 kHz");
    } else {
        m_acqButton->setText("▶  DÉMARRER ÉMISSION");
        m_acqButton->setStyleSheet("");
        m_acqButton->setProperty("cssClass","connect");
        m_acqButton->style()->unpolish(m_acqButton);
        m_acqButton->style()->polish(m_acqButton);
        m_ledTimer->stop();
        m_ledLabel->setStyleSheet("color:#444c56;font-size:16px;");
        appendLog("warn","Émission acoustique arrêtée");
    }
}

void MainWindow::blinkLed()
{
    m_ledOn = !m_ledOn;
    m_ledLabel->setStyleSheet(
        m_ledOn?"color:#00e676;font-size:16px;":"color:#444c56;font-size:16px;");
}

void MainWindow::setupVoieBar()
{
    auto* layout = qobject_cast<QHBoxLayout*>(ui->voieBar->layout());
    struct VoieDef { QString key,label,color; };
    QList<VoieDef> hydros={{"h1","H1","#00d4ff"},{"h2","H2","#00e676"},
                            {"h3","H3","#ffaa00"},{"h4","H4","#ff6b9d"}};
    QList<VoieDef> amplis={{"a1","A1","#c792ea"},{"a2","A2","#89ddff"}};
    auto addLbl=[&](const QString& t){
        auto* l=new QLabel(t);
        l->setStyleSheet("color:#6e7681;font-size:13px;font-weight:bold;");
        layout->addWidget(l);
    };
    auto addBtn=[&](const VoieDef& d){
        auto* btn=new QPushButton(d.label);
        btn->setCheckable(true); btn->setChecked(true);
        btn->setFixedHeight(26); btn->setMinimumWidth(44);
        btn->setStyleSheet(QString(
            "QPushButton{border:1px solid %1;color:%1;background:transparent;"
            "font-size:13px;font-weight:bold;border-radius:2px;padding:2px 8px;}"
            "QPushButton:checked{background:%2;}"
            "QPushButton:!checked{color:#444c56;border-color:#444c56;}"
        ).arg(d.color,d.color+"22"));
        btn->setProperty("voieKey",d.key);
        connect(btn,&QPushButton::toggled,this,&MainWindow::onToggleVoie);
        layout->addWidget(btn); m_voieBtns[d.key]=btn;
    };
    auto addSep=[&](){
        auto* s=new QFrame(); s->setFrameShape(QFrame::VLine);
        s->setStyleSheet("color:#30363d;"); layout->addWidget(s);
    };
    addLbl("HYDROS :"); for(auto& d:hydros) addBtn(d);
    addSep();
    addLbl("AMPLIS :"); for(auto& d:amplis) addBtn(d);
    addSep();
    auto* info=new QLabel(
        "Fs <span style='color:#00d4ff;font-weight:bold'>96 kHz</span>"
        " &nbsp;|&nbsp; MLI "
        "<span style='color:#00e676;font-weight:bold'>38.4 kHz</span>");
    info->setTextFormat(Qt::RichText);
    info->setStyleSheet("font-size:13px;color:#8b949e;");
    layout->addWidget(info); layout->addStretch();
}

void MainWindow::setupCharts()
{
    auto* grid=qobject_cast<QGridLayout*>(ui->chartsContainer->layout());
    auto makeBox=[](const QString& title, GraphWidget* gw){
        auto* box=new QGroupBox(title);
        auto* l=new QVBoxLayout(box); l->setContentsMargins(4,18,4,4);
        l->addWidget(gw); return box;
    };
    m_graphTime   = new GraphWidget(GraphWidget::TimeDomain,  this);
    m_graphFFT    = new GraphWidget(GraphWidget::FFT,         this);
    m_graphLevels = new GraphWidget(GraphWidget::Levels,      this);
    m_graphSpec   = new GraphWidget(GraphWidget::Spectrogram, this);
    grid->addWidget(makeBox("Signal temporel — hydrophones", m_graphTime),   0,0);
    grid->addWidget(makeBox("Spectrogramme — fréq. / temps", m_graphSpec),   0,1);
    grid->addWidget(makeBox("FFT",                           m_graphFFT),    1,0);
    grid->addWidget(makeBox("Niveaux — dB re 1µPa",         m_graphLevels), 1,1);
    grid->setRowStretch(0,1); grid->setRowStretch(1,1);
}

void MainWindow::setupSidebar()
{
    auto* scroll=new QScrollArea();
    scroll->setWidgetResizable(true); scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("background:#161b22;border-left:1px solid #30363d;");
    auto* container=new QWidget();
    auto* layout=new QVBoxLayout(container);
    layout->setSpacing(0); layout->setContentsMargins(0,0,0,0);
    auto addTextSection=[&](const QString& title,
                             QList<QPair<QString,QString>> rows,
                             QList<QString> states){
        auto* box=new QGroupBox(title);
        auto* l=new QVBoxLayout(box); l->setSpacing(3); l->setContentsMargins(8,18,8,6);
        for(int i=0;i<rows.size();i++){
            auto& row=rows[i];
            auto* hl=new QHBoxLayout();
            auto* lbl=new QLabel(row.first); lbl->setStyleSheet("font-size:13px;color:#8b949e;");
            auto* val=new QLabel(row.second);
            QString st=i<states.size()?states[i]:"ok";
            val->setStyleSheet(
                st=="warn"?"font-weight:bold;font-size:14px;color:#ffaa00;":
                st=="err" ?"font-weight:bold;font-size:14px;color:#ff4444;":
                st=="info"?"font-weight:bold;font-size:14px;color:#00d4ff;":
                           "font-weight:bold;font-size:14px;color:#00e676;");
            val->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
            hl->addWidget(lbl); hl->addWidget(val); l->addLayout(hl);
        }
        layout->addWidget(box);
    };
    auto addMiniSection=[&](const QString& title, GraphWidget*& gw,
                             const QColor& col, const QString& unit,
                             const QString& label, double lo, double hi){
        auto* box=new QGroupBox(title); box->setFixedHeight(90);
        auto* l=new QVBoxLayout(box); l->setContentsMargins(2,16,2,2);
        gw=new GraphWidget(GraphWidget::MiniPlot,this);
        gw->setMiniPlotColor(col); gw->setMiniPlotUnit(unit);
        gw->setMiniPlotLabel(label); gw->setMiniPlotRange(lo,hi);
        l->addWidget(gw); layout->addWidget(box);
    };
    addTextSection("Amplificateurs",{{"AMP 1","ACTIF"},{"AMP 2","STANDBY"}},{"ok","warn"});
    addTextSection("Hydrophones",
        {{"Hydro 1","ACQ"},{"Hydro 2","ACQ"},{"Hydro 3","BRUIT↑"},{"Hydro 4","DÉCO"}},
        {"ok","ok","warn","err"});
    addMiniSection("Température T1 (K)",m_miniTemp1,QColor("#00e676"),"°C","T1",0,80);
    addMiniSection("Température T2 (J)",m_miniTemp2,QColor("#ffaa00"),"°C","T2",0,80);
    addMiniSection("V Bus 48V",         m_miniV1,   QColor("#00d4ff"),"V", "VBus",40,55);
    addMiniSection("V Alim 12V",        m_miniV2,   QColor("#89ddff"),"V", "V12",10,15);
    addMiniSection("Courant I AMP1",    m_miniI1,   QColor("#ffaa00"),"A", "I1",0,6);
    addMiniSection("Courant I AMP2",    m_miniI2,   QColor("#c792ea"),"A", "I2",0,6);
    addTextSection("Mesure Sv",
        {{"Gîte","127.5°"},{"Site","32.0°"},{"Sv moyen","−18.4 dB"},{"Progression","35%"}},
        {"info","info","info","info"});
    layout->addStretch();
    scroll->setWidget(container);
    auto* sl=qobject_cast<QVBoxLayout*>(ui->sidebar->layout());
    ui->sidebar->setStyleSheet("background:#161b22;border-left:1px solid #30363d;");
    sl->addWidget(scroll);
}

void MainWindow::setupLogView()
{
    ui->logView->setReadOnly(true);
    ui->logView->document()->setMaximumBlockCount(200);
}

void MainWindow::appendLog(const QString& level, const QString& msg)
{
    QString ts=QDateTime::currentDateTime().toString("hh:mm:ss");
    QString color=(level=="ok")?"#00e676":(level=="warn")?"#ffaa00":"#ff4444";
    ui->logView->append(
        QString("<span style='color:#6e7681;font-size:12px'>%1</span>"
                "&nbsp;&nbsp;<span style='color:%2;font-size:13px'>%3</span>")
        .arg(ts,color,msg));
}

void MainWindow::setupStatusBar()
{
    statusBar()->showMessage(
        QString("Connecté — %1 · NI PXIe-8301 · Fs 96 kHz").arg(m_connConfig.chassis));
}

void MainWindow::onToggleVoie()
{
    auto* btn=qobject_cast<QPushButton*>(sender()); if(!btn) return;
    QString key=btn->property("voieKey").toString(); bool on=btn->isChecked();
    m_voies[key]=on;
    if(m_graphTime)   m_graphTime->setVoieActive(key,on);
    if(m_graphLevels) m_graphLevels->setVoieActive(key,on);
    if(m_graphSpec)   m_graphSpec->setVoieActive(key,on);
}

void MainWindow::updateMetrics()
{
    auto* rng=QRandomGenerator::global();
    if(m_miniTemp1) m_miniTemp1->pushMiniValue(24.0+rng->generateDouble()*0.8);
    if(m_miniTemp2) m_miniTemp2->pushMiniValue(47.0+rng->generateDouble()*2.0);
    if(m_miniV1)    m_miniV1->pushMiniValue(48.0+rng->generateDouble()*0.6-0.3);
    if(m_miniV2)    m_miniV2->pushMiniValue(12.0+rng->generateDouble()*0.3-0.15);
    if(m_miniI1)    m_miniI1->pushMiniValue(3.5+rng->generateDouble()*0.8);
    if(m_miniI2)    m_miniI2->pushMiniValue(0.3+rng->generateDouble()*0.2);
}

void MainWindow::toggleTheme()
{
    auto& sm=StyleManager::instance();
    sm.setTheme(sm.isDark()?StyleManager::Light:StyleManager::Dark);
    setStyleSheet(sm.mainStyleSheet());
    if(m_configWindow) m_configWindow->setStyleSheet(sm.configStyleSheet());
    update();
}

void MainWindow::openConfig()
{
    try {
        if(!m_configWindow){
            m_configWindow=new ConfigWindow(nullptr);
            m_configWindow->setStyleSheet(StyleManager::instance().configStyleSheet());
            m_configWindow->setAttribute(Qt::WA_DeleteOnClose,false);
            m_configWindow->resize(1100,750);
            m_configWindow->move(this->x()+60,this->y()+40);
            connect(m_configWindow,&QObject::destroyed,
                    this,[this](){m_configWindow=nullptr;});
        }
        m_configWindow->show();
        m_configWindow->raise();
        m_configWindow->activateWindow();
    } catch(...){ qDebug()<<"Erreur ConfigWindow"; }
}



