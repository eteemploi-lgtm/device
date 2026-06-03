#pragma once
#include <QWidget>
#include <QTimer>
#include <QMap>
#include <vector>
#include <deque>

class GraphWidget : public QWidget
{
    Q_OBJECT

public:
    enum GraphType { TimeDomain, FFT, Levels, Spectrogram, MiniPlot };

    explicit GraphWidget(GraphType type, QWidget* parent = nullptr);

    void setVoieActive(const QString& key, bool active);
    bool isDark() const;

    // MiniPlot : configuration
    void setMiniPlotColor(const QColor& c)      { m_miniColor = c; }
    void setMiniPlotUnit(const QString& u)       { m_miniUnit  = u; }
    void setMiniPlotRange(double lo, double hi)  { m_miniLo=lo; m_miniHi=hi; }
    void setMiniPlotLabel(const QString& l)      { m_miniLabel = l; }
    void pushMiniValue(double v);

protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override { update(); }

private slots:
    void onTimer();

private:
    void drawTimeDomain(QPainter& p);
    void drawFFT(QPainter& p);
    void drawLevels(QPainter& p);
    void drawSpectrogram(QPainter& p);
    void drawMiniPlot(QPainter& p);
    void drawGrid(QPainter& p, int cols, int rows,
                  int offL=0, int offR=0, int offT=0, int offB=0);
    void updateSpectroBuffer();

    GraphType m_type;
    QTimer*   m_timer;
    double    m_T = 0.0;

    QMap<QString,bool>   m_voies;
    QMap<QString,QColor> m_colors;

    std::vector<std::vector<double>> m_spectroBuffer;

    // MiniPlot
    std::deque<double> m_miniData;
    QColor  m_miniColor  = QColor("#00d4ff");
    QString m_miniUnit   = "";
    QString m_miniLabel  = "";
    double  m_miniLo     = 0.0;
    double  m_miniHi     = 100.0;
    int     m_miniMaxPts = 120;
};



#include "GraphWidget.h"
#include "StyleManager.h"

#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <QRandomGenerator>
#include <QtMath>

static const QMap<QString,QColor> COLORS = {
    {"h1", QColor("#00d4ff")},
    {"h2", QColor("#00e676")},
    {"h3", QColor("#ffaa00")},
    {"h4", QColor("#ff6b9d")},
    {"a1", QColor("#c792ea")},
    {"a2", QColor("#89ddff")},
};

GraphWidget::GraphWidget(GraphType type, QWidget* parent)
    : QWidget(parent), m_type(type)
{
    for (auto& k : COLORS.keys()) m_voies[k] = true;
    m_colors = COLORS;
    m_spectroBuffer.assign(300, std::vector<double>(100, 0.0));

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &GraphWidget::onTimer);
    m_timer->start(33);
    setMinimumSize(200, 80);
}

void GraphWidget::setVoieActive(const QString& key, bool active)
{
    m_voies[key] = active;
}

bool GraphWidget::isDark() const
{
    return StyleManager::instance().isDark();
}

void GraphWidget::onTimer()
{
    m_T += 1.0;
    if (m_type == Spectrogram) updateSpectroBuffer();
    update();
}

void GraphWidget::updateSpectroBuffer()
{
    int cols = m_spectroBuffer.size();
    int rows = m_spectroBuffer[0].size();
    for (int x = 0; x < cols-1; x++)
        m_spectroBuffer[x] = m_spectroBuffer[x+1];
    for (int y = 0; y < rows; y++) {
        double freq = double(y)/rows;
        double energy =
            0.75*qExp(-qPow((freq-0.40)/0.10,2))
          + 0.35*qExp(-qPow((freq-0.20)/0.07,2))
          + 0.20*qExp(-qPow((freq-0.65)/0.05,2))
          + 0.05*QRandomGenerator::global()->generateDouble()
          + 0.08*qSin(m_T*0.03+freq*10.0);
        m_spectroBuffer[cols-1][y] = qBound(0.0,energy,1.0);
    }
}

static QColor thermalColor(double v)
{
    v = qBound(0.0,v,1.0);
    if      (v < 0.20) return QColor::fromHsvF(0.67,1.0,v*5.0);
    else if (v < 0.40) return QColor::fromHsvF(0.55,1.0,1.0);
    else if (v < 0.60) return QColor::fromHsvF(0.38,1.0,1.0);
    else if (v < 0.80) return QColor::fromHsvF(0.17,1.0,1.0);
    else               return QColor::fromHsvF(0.0, 1.0,1.0);
}

void GraphWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing,
                    m_type!=Levels && m_type!=Spectrogram);
    QColor bg = isDark() ? QColor("#0d1117") : QColor("#f8fafc");
    p.fillRect(rect(), bg);
    switch (m_type) {
        case TimeDomain:  drawTimeDomain(p);  break;
        case FFT:         drawFFT(p);         break;
        case Levels:      drawLevels(p);      break;
        case Spectrogram: drawSpectrogram(p); break;
        case MiniPlot:    drawMiniPlot(p);    break;
    }
}

void GraphWidget::drawGrid(QPainter& p, int cols, int rows,
                            int offL, int offR, int offT, int offB)
{
    QColor gc = isDark() ? QColor("#1f2937") : QColor("#e2e8f0");
    p.setPen(QPen(gc,0.5));
    int W=width()-offL-offR, H=height()-offT-offB;
    for (int i=1;i<rows;i++) p.drawLine(offL,offT+H*i/rows,offL+W,offT+H*i/rows);
    for (int i=1;i<cols;i++) p.drawLine(offL+W*i/cols,offT,offL+W*i/cols,offT+H);
}

void GraphWidget::drawTimeDomain(QPainter& p)
{
    const int OL=38,OR=6,OT=6,OB=18;
    drawGrid(p,8,4,OL,OR,OT,OB);
    int W=width()-OL-OR, H=height()-OT-OB;
    QColor tc=isDark()?QColor("#6e7681"):QColor("#718096");
    p.setPen(tc); p.setFont(QFont("Courier New",8));
    p.drawText(2,OT+4,"+1V"); p.drawText(2,OT+H/2+4," 0V"); p.drawText(2,OT+H-2,"-1V");
    QStringList xl={"0","0.25","0.50","0.75","1ms"};
    for(int i=0;i<xl.size();i++) p.drawText(OL+W*i/(xl.size()-1)-10,height()-2,xl[i]);

    static const double phases[]={0,0.8,1.6,2.4};
    static const double freqs[] ={1.0,1.3,0.9,1.6};
    static const double amps[]  ={0.25,0.19,0.28,0.15};
    for(int vi=1;vi<=4;vi++){
        QString key=QString("h%1").arg(vi);
        if(!m_voies.value(key,false)) continue;
        QColor col=m_colors.value(key);
        p.setPen(QPen(col,1.5));
        QPainterPath path;
        double yBase=OT+H*(0.12+(vi-1)*0.19)+H*0.06;
        for(int x=0;x<W;x++){
            double tt=(double(x)/W)*6*M_PI+m_T*0.04+phases[vi-1];
            double mod=0.7+0.3*qSin(m_T*0.015+vi);
            double y=yBase-qSin(tt*freqs[vi-1])*H*amps[vi-1]*mod;
            x==0?path.moveTo(OL+x,y):path.lineTo(OL+x,y);
        }
        p.drawPath(path);
        p.setPen(col); p.setFont(QFont("Courier New",9,QFont::Bold));
        p.drawText(OL+4,int(yBase-H*amps[vi-1]-2),key.toUpper());
    }
}

void GraphWidget::drawFFT(QPainter& p)
{
    const int OL=40,OR=6,OT=6,OB=18;
    drawGrid(p,8,4,OL,OR,OT,OB);
    int W=width()-OL-OR, H=height()-OT-OB;
    QColor tc=isDark()?QColor("#6e7681"):QColor("#718096");
    p.setPen(tc); p.setFont(QFont("Courier New",8));
    p.drawText(2,OT+4," 0dB"); p.drawText(2,OT+H/4+4,"-20");
    p.drawText(2,OT+H/2+4,"-40"); p.drawText(2,OT+3*H/4+4,"-60");
    p.drawText(2,OT+H-2,"-80");
    QStringList fl={"0","12","24","36","48","60","72","84","96kHz"};
    for(int i=0;i<fl.size();i++) p.drawText(OL+W*i/(fl.size()-1)-10,height()-2,fl[i]);
    const int BINS=64;
    double bw=double(W)/BINS;
    for(int i=0;i<BINS;i++){
        double noise=QRandomGenerator::global()->generateDouble()*0.06;
        double peak=(i==38)?0.90+noise:(i==19)?0.42+noise:(i==57)?0.28+noise:noise;
        double env=qExp(-qPow((i-38.0)/11.0,2))*0.55+noise;
        double bh=(peak+env)*H*0.88;
        QColor col=(i==38)?QColor("#00d4ff"):(i==19)?QColor("#00e676"):QColor("#444c56");
        p.fillRect(QRectF(OL+i*bw,OT+H-bh,bw-1,bh),col);
    }
    p.setPen(QColor("#00d4ff")); p.setFont(QFont("Courier New",9,QFont::Bold));
    p.drawText(OL+int(38*W/64)-4,OT+int(H*0.08),"38.4 kHz");
}

void GraphWidget::drawLevels(QPainter& p)
{
    const int OL=44,OR=6,OT=6,OB=20;
    int W=width()-OL-OR, H=height()-OT-OB;
    QColor bgBar=isDark()?QColor("#1f2937"):QColor("#e2e8f0");
    QColor gc=isDark()?QColor("#1f2937"):QColor("#e2e8f0");
    p.setPen(QPen(gc,0.5));
    for(int i=1;i<5;i++) p.drawLine(OL,OT+H*i/4,OL+W,OT+H*i/4);
    QColor tc=isDark()?QColor("#6e7681"):QColor("#718096");
    p.setPen(tc); p.setFont(QFont("Courier New",8));
    QStringList db={"+20","+10"," 0","-10","-20"};
    for(int i=0;i<5;i++) p.drawText(2,OT+H*i/4+4,db[i]);
    static const QMap<QString,double> BASE={
        {"h1",0.75},{"h2",0.62},{"h3",0.88},{"h4",0.12},{"a1",0.55},{"a2",0.45}};
    QStringList active;
    for(auto& k:COLORS.keys()) if(m_voies.value(k,false)) active<<k;
    if(active.isEmpty()) return;
    int barW=qMin(40,W/active.size()-8);
    int spacing=(W-active.size()*barW)/(active.size()+1);
    for(int i=0;i<active.size();i++){
        const QString& k=active[i];
        int x=OL+spacing+i*(barW+spacing);
        double lv=BASE.value(k,0.5)+0.04*qSin(m_T*0.07+i);
        int bh=int(lv*(H-4));
        p.fillRect(x,OT+4,barW,H-4,bgBar);
        QColor col=m_colors.value(k); col.setAlphaF(0.9);
        p.fillRect(x,OT+H-bh,barW,bh,col);
        double dbv=-20.0+lv*40.0;
        p.setPen(m_colors.value(k)); p.setFont(QFont("Courier New",8,QFont::Bold));
        p.drawText(QRect(x,OT+H-bh-14,barW,12),Qt::AlignCenter,QString::number(dbv,'f',1));
        p.drawText(QRect(x,height()-16,barW,14),Qt::AlignCenter,k.toUpper());
    }
}

void GraphWidget::drawSpectrogram(QPainter& p)
{
    const int OL=44,OR=22,OT=6,OB=18;
    int W=width()-OL-OR, H=height()-OT-OB;
    if(W<=0||H<=0) return;
    int cols=m_spectroBuffer.size(), rows=m_spectroBuffer[0].size();
    for(int x=0;x<W;x++){
        int ci=x*cols/W;
        for(int y=0;y<H;y++){
            int ri=y*rows/H;
            p.fillRect(OL+x,OT+H-1-y,1,1,thermalColor(m_spectroBuffer[ci][ri]));
        }
    }
    int yPeak=OT+H-int(0.40*H);
    p.setPen(QPen(QColor(255,255,255,100),1,Qt::DashLine));
    p.drawLine(OL,yPeak,OL+W,yPeak);
    p.setPen(QColor(255,255,255,200)); p.setFont(QFont("Courier New",8,QFont::Bold));
    p.drawText(OL+3,yPeak-2,"38.4kHz");
    QColor tc=isDark()?QColor("#8b949e"):QColor("#4a5568");
    p.setPen(tc); p.setFont(QFont("Courier New",8));
    p.drawText(2,OT+4,"96k"); p.drawText(2,OT+H/4+4,"72k");
    p.drawText(2,OT+H/2+4,"48k"); p.drawText(2,OT+3*H/4+4,"24k");
    p.drawText(2,OT+H-2," 0Hz");
    p.drawText(OL,height()-2,"−t"); p.drawText(OL+W-8,height()-2,"0s");
    int bx=OL+W+4,bw2=14;
    for(int y=0;y<H;y++){
        double v=1.0-double(y)/H;
        p.fillRect(bx,OT+y,bw2,1,thermalColor(v));
    }
    p.setPen(QColor("#6e7681")); p.setFont(QFont("Courier New",7));
    p.drawText(bx,OT+6,"Hi"); p.drawText(bx,OT+H/2+4,"Mid"); p.drawText(bx,OT+H-2,"Lo");
    p.setPen(QPen(QColor("#444c56"),1)); p.drawRect(bx,OT,bw2,H);
}

// ─── MiniPlot ────────────────────────────────────────────────────────────────
void GraphWidget::pushMiniValue(double v)
{
    m_miniData.push_back(v);
    while((int)m_miniData.size()>m_miniMaxPts) m_miniData.pop_front();
}

void GraphWidget::drawMiniPlot(QPainter& p)
{
    const int OL=36,OR=4,OT=16,OB=14;
    int W=width()-OL-OR, H=height()-OT-OB;
    if(W<=0||H<=0||m_miniData.empty()) return;

    QColor bg=isDark()?QColor("#0d1117"):QColor("#f8fafc");
    p.fillRect(rect(),bg);

    // Grille
    QColor gc=isDark()?QColor("#1f2937"):QColor("#e2e8f0");
    p.setPen(QPen(gc,0.5));
    for(int i=1;i<4;i++) p.drawLine(OL,OT+H*i/4,OL+W,OT+H*i/4);

    double cur=m_miniData.back();

    // Valeur courante
    p.setPen(m_miniColor);
    p.setFont(QFont("Courier New",11,QFont::Bold));
    p.drawText(QRect(0,0,OL-2,OT+4),
               Qt::AlignRight|Qt::AlignVCenter,
               QString::number(cur,'f',1));

    // Unité + label
    p.setPen(isDark()?QColor("#6e7681"):QColor("#718096"));
    p.setFont(QFont("Courier New",8));
    p.drawText(2,OT-2,m_miniUnit);
    p.drawText(OL,height()-2,m_miniLabel);

    // Axe Y
    p.setPen(isDark()?QColor("#444c56"):QColor("#a0aec0"));
    p.setFont(QFont("Courier New",7));
    p.drawText(2,OT+6,   QString::number(m_miniHi,'f',0));
    p.drawText(2,OT+H-2, QString::number(m_miniLo,'f',0));

    // Courbe
    p.setPen(QPen(m_miniColor,1.5));
    QPainterPath path;
    int n=m_miniData.size();
    for(int i=0;i<n;i++){
        double nx=OL+double(i)/(m_miniMaxPts-1)*W;
        double ny=OT+H-(m_miniData[i]-m_miniLo)/(m_miniHi-m_miniLo)*H;
        ny=qBound(double(OT),ny,double(OT+H));
        i==0?path.moveTo(nx,ny):path.lineTo(nx,ny);
    }
    p.drawPath(path);

    // Remplissage
    if(n>1){
        QPainterPath fill=path;
        fill.lineTo(OL+double(n-1)/(m_miniMaxPts-1)*W,OT+H);
        fill.lineTo(OL,OT+H); fill.closeSubpath();
        QColor fc=m_miniColor; fc.setAlphaF(0.12);
        p.fillPath(fill,fc);
    }

    // Point courant
    double cx=OL+double(n-1)/(m_miniMaxPts-1)*W;
    double cy=OT+H-(cur-m_miniLo)/(m_miniHi-m_miniLo)*H;
    cy=qBound(double(OT),cy,double(OT+H));
    p.setPen(Qt::NoPen); p.setBrush(m_miniColor);
    p.drawEllipse(QPointF(cx,cy),3,3);
}





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
    m_metricTimer->start(400);

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
    if (m_configWindow) { m_configWindow->close(); m_configWindow = nullptr; }
    delete ui;
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
    connect(mHelp->addAction("À propos..."), &QAction::triggered, this, [this](){
        QDialog* dlg = new QDialog(this);
        dlg->setWindowTitle("À propos — ACOUSTIMETER");
        dlg->setFixedSize(360, 220);
        dlg->setStyleSheet(StyleManager::instance().appStyleSheet());
        auto* l = new QVBoxLayout(dlg);
        l->setSpacing(10); l->setContentsMargins(24,20,24,20);
        auto* logo = new QLabel("ACOUSTIMETER");
        logo->setObjectName("logoLabel");
        logo->setAlignment(Qt::AlignCenter);
        l->addWidget(logo);
        auto* sub = new QLabel("Logiciel de pilotage et mesure acoustique");
        sub->setAlignment(Qt::AlignCenter);
        sub->setObjectName("subLabel");
        l->addWidget(sub);
        l->addWidget(new QLabel("Version : 1.0"));
        l->addWidget(new QLabel("Matériel : NI PXIe-8301"));
        l->addWidget(new QLabel("Qt : " + QString(QT_VERSION_STR)));
        auto* ok = new QPushButton("Fermer");
        ok->setProperty("cssClass","primary");
        connect(ok, &QPushButton::clicked, dlg, &QDialog::accept);
        l->addWidget(ok);
        dlg->exec();
    });

    // Toolbar
    auto* tb = addToolBar("main");
    tb->setMovable(false);

    auto* logoLbl = new QLabel("  ACOUSTIMETER  ");
    logoLbl->setStyleSheet(
        "color:#00d4ff;font-size:16px;font-weight:bold;letter-spacing:2px;");
    tb->addWidget(logoLbl);
    tb->addSeparator();

    // Bouton DÉMARRER/ARRÊTER avec LED animée
    m_ledLabel = new QLabel("●");
    m_ledLabel->setStyleSheet("color:#444c56; font-size:16px;");
    m_ledLabel->setFixedWidth(20);
    tb->addWidget(m_ledLabel);

    m_acqButton = new QPushButton("▶  DÉMARRER ÉMISSION");
    m_acqButton->setProperty("cssClass","connect");
    m_acqButton->setFixedHeight(30);
    m_acqButton->setMinimumWidth(180);
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
    cfgBtn->setFixedHeight(30);
    cfgBtn->setMinimumWidth(100);
    connect(cfgBtn, &QPushButton::clicked, this, &MainWindow::openConfig);
    tb->addWidget(cfgBtn);
    tb->addSeparator();

    // Timer LED clignotante
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
        appendLog("ok", "Émission acoustique démarrée — MLI 38.4 kHz");
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
        m_ledOn ? "color:#00e676;font-size:16px;" : "color:#444c56;font-size:16px;");
}

void MainWindow::setupVoieBar()
{
    auto* layout = qobject_cast<QHBoxLayout*>(ui->voieBar->layout());

    struct VoieDef { QString key; QString label; QString color; };
    QList<VoieDef> hydros = {
        {"h1","H1","#00d4ff"},{"h2","H2","#00e676"},
        {"h3","H3","#ffaa00"},{"h4","H4","#ff6b9d"}
    };
    QList<VoieDef> amplis = {
        {"a1","A1","#c792ea"},{"a2","A2","#89ddff"}
    };

    auto addLabel = [&](const QString& txt){
        auto* l = new QLabel(txt);
        l->setStyleSheet("color:#6e7681;font-size:13px;font-weight:bold;");
        layout->addWidget(l);
    };
    auto addBtn = [&](const VoieDef& d){
        auto* btn = new QPushButton(d.label);
        btn->setCheckable(true); btn->setChecked(true);
        btn->setFixedHeight(26); btn->setMinimumWidth(44);
        btn->setStyleSheet(QString(
            "QPushButton{border:1px solid %1;color:%1;background:transparent;"
            "font-size:13px;font-weight:bold;border-radius:2px;padding:2px 8px;}"
            "QPushButton:checked{background:%2;}"
            "QPushButton:!checked{color:#444c56;border-color:#444c56;}"
        ).arg(d.color, d.color+"22"));
        btn->setProperty("voieKey",d.key);
        connect(btn,&QPushButton::toggled,this,&MainWindow::onToggleVoie);
        layout->addWidget(btn);
        m_voieBtns[d.key]=btn;
    };
    auto addSep=[&](){
        auto* s=new QFrame(); s->setFrameShape(QFrame::VLine);
        s->setStyleSheet("color:#30363d;"); layout->addWidget(s);
    };

    addLabel("HYDROS :");
    for(auto& d:hydros) addBtn(d);
    addSep();
    addLabel("AMPLIS :");
    for(auto& d:amplis) addBtn(d);
    addSep();
    auto* info=new QLabel(
        "Fs <span style='color:#00d4ff;font-weight:bold'>96 kHz</span>"
        " &nbsp;|&nbsp; MLI "
        "<span style='color:#00e676;font-weight:bold'>38.4 kHz</span>");
    info->setTextFormat(Qt::RichText);
    info->setStyleSheet("font-size:13px;color:#8b949e;");
    layout->addWidget(info);
    layout->addStretch();
}

void MainWindow::setupCharts()
{
    auto* grid = qobject_cast<QGridLayout*>(ui->chartsContai











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
    m_metricTimer->start(400);

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
    if (m_configWindow) { m_configWindow->close(); m_configWindow = nullptr; }
    delete ui;
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
    connect(mHelp->addAction("À propos..."), &QAction::triggered, this, [this](){
        QDialog* dlg = new QDialog(this);
        dlg->setWindowTitle("À propos — ACOUSTIMETER");
        dlg->setFixedSize(360, 220);
        dlg->setStyleSheet(StyleManager::instance().appStyleSheet());
        auto* l = new QVBoxLayout(dlg);
        l->setSpacing(10); l->setContentsMargins(24,20,24,20);
        auto* logo = new QLabel("ACOUSTIMETER");
        logo->setObjectName("logoLabel");
        logo->setAlignment(Qt::AlignCenter);
        l->addWidget(logo);
        auto* sub = new QLabel("Logiciel de pilotage et mesure acoustique");
        sub->setAlignment(Qt::AlignCenter);
        sub->setObjectName("subLabel");
        l->addWidget(sub);
        l->addWidget(new QLabel("Version : 1.0"));
        l->addWidget(new QLabel("Matériel : NI PXIe-8301"));
        l->addWidget(new QLabel("Qt : " + QString(QT_VERSION_STR)));
        auto* ok = new QPushButton("Fermer");
        ok->setProperty("cssClass","primary");
        connect(ok, &QPushButton::clicked, dlg, &QDialog::accept);
        l->addWidget(ok);
        dlg->exec();
    });

    // Toolbar
    auto* tb = addToolBar("main");
    tb->setMovable(false);

    auto* logoLbl = new QLabel("  ACOUSTIMETER  ");
    logoLbl->setStyleSheet(
        "color:#00d4ff;font-size:16px;font-weight:bold;letter-spacing:2px;");
    tb->addWidget(logoLbl);
    tb->addSeparator();

    // Bouton DÉMARRER/ARRÊTER avec LED animée
    m_ledLabel = new QLabel("●");
    m_ledLabel->setStyleSheet("color:#444c56; font-size:16px;");
    m_ledLabel->setFixedWidth(20);
    tb->addWidget(m_ledLabel);

    m_acqButton = new QPushButton("▶  DÉMARRER ÉMISSION");
    m_acqButton->setProperty("cssClass","connect");
    m_acqButton->setFixedHeight(30);
    m_acqButton->setMinimumWidth(180);
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
    cfgBtn->setFixedHeight(30);
    cfgBtn->setMinimumWidth(100);
    connect(cfgBtn, &QPushButton::clicked, this, &MainWindow::openConfig);
    tb->addWidget(cfgBtn);
    tb->addSeparator();

    // Timer LED clignotante
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
        appendLog("ok", "Émission acoustique démarrée — MLI 38.4 kHz");
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
        m_ledOn ? "color:#00e676;font-size:16px;" : "color:#444c56;font-size:16px;");
}

void MainWindow::setupVoieBar()
{
    auto* layout = qobject_cast<QHBoxLayout*>(ui->voieBar->layout());

    struct VoieDef { QString key; QString label; QString color; };
    QList<VoieDef> hydros = {
        {"h1","H1","#00d4ff"},{"h2","H2","#00e676"},
        {"h3","H3","#ffaa00"},{"h4","H4","#ff6b9d"}
    };
    QList<VoieDef> amplis = {
        {"a1","A1","#c792ea"},{"a2","A2","#89ddff"}
    };

    auto addLabel = [&](const QString& txt){
        auto* l = new QLabel(txt);
        l->setStyleSheet("color:#6e7681;font-size:13px;font-weight:bold;");
        layout->addWidget(l);
    };
    auto addBtn = [&](const VoieDef& d){
        auto* btn = new QPushButton(d.label);
        btn->setCheckable(true); btn->setChecked(true);
        btn->setFixedHeight(26); btn->setMinimumWidth(44);
        btn->setStyleSheet(QString(
            "QPushButton{border:1px solid %1;color:%1;background:transparent;"
            "font-size:13px;font-weight:bold;border-radius:2px;padding:2px 8px;}"
            "QPushButton:checked{background:%2;}"
            "QPushButton:!checked{color:#444c56;border-color:#444c56;}"
        ).arg(d.color, d.color+"22"));
        btn->setProperty("voieKey",d.key);
        connect(btn,&QPushButton::toggled,this,&MainWindow::onToggleVoie);
        layout->addWidget(btn);
        m_voieBtns[d.key]=btn;
    };
    auto addSep=[&](){
        auto* s=new QFrame(); s->setFrameShape(QFrame::VLine);
        s->setStyleSheet("color:#30363d;"); layout->addWidget(s);
    };

    addLabel("HYDROS :");
    for(auto& d:hydros) addBtn(d);
    addSep();
    addLabel("AMPLIS :");
    for(auto& d:amplis) addBtn(d);
    addSep();
    auto* info=new QLabel(
        "Fs <span style='color:#00d4ff;font-weight:bold'>96 kHz</span>"
        " &nbsp;|&nbsp; MLI "
        "<span style='color:#00e676;font-weight:bold'>38.4 kHz</span>");
    info->setTextFormat(Qt::RichText);
    info->setStyleSheet("font-size:13px;color:#8b949e;");
    layout->addWidget(info);
    layout->addStretch();
}

void MainWindow::setupCharts()
{
    auto* grid = qobject_cast<QGridLayout*>(ui->chartsContainer->layout());
    auto makeBox = [](const QString& title, GraphWidget* gw){
        auto* box = new QGroupBox(title);
        auto* l = new QVBoxLayout(box);
        l->setContentsMargins(4,18,4,4);
        l->addWidget(gw);
        return box;
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
    auto* scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("background:#161b22;border-left:1px solid #30363d;");

    auto* container = new QWidget();
    auto* layout = new QVBoxLayout(container);
    layout->setSpacing(0); layout->setContentsMargins(0,0,0,0);

    // ── Amplificateurs & Hydrophones (texte) ──
    auto addTextSection = [&](const QString& title,
                               QList<QPair<QString,QString>> rows,
                               QList<QString> states){
        auto* box = new QGroupBox(title);
        auto* l = new QVBoxLayout(box);
        l->setSpacing(3); l->setContentsMargins(8,18,8,6);
        for(int i=0;i<rows.size();i++){
            auto& row=rows[i];
            auto* hl=new QHBoxLayout();
            auto* lbl=new QLabel(row.first);
            lbl->setStyleSheet("font-size:13px;color:#8b949e;");
            auto* val=new QLabel(row.second);
            QString st=i<states.size()?states[i]:"ok";
            val->setStyleSheet(
                st=="warn"?"font-weight:bold;font-size:14px;color:#ffaa00;":
                st=="err" ?"font-weight:bold;font-size:14px;color:#ff4444;":
                st=="info"?"font-weight:bold;font-size:14px;color:#00d4ff;":
                           "font-weight:bold;font-size:14px;color:#00e676;");
            val->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
            hl->addWidget(lbl); hl->addWidget(val);
            l->addLayout(hl);
        }
        layout->addWidget(box);
    };

    addTextSection("Amplificateurs",
        {{"AMP 1","ACTIF"},{"AMP 2","STANDBY"}},{"ok","warn"});
    addTextSection("Hydrophones",
        {{"Hydro 1","ACQ"},{"Hydro 2","ACQ"},
         {"Hydro 3","BRUIT↑"},{"Hydro 4","DÉCO"}},
        {"ok","ok","warn","err"});

    // ── MiniPlots courbes défilantes ──
    auto addMiniSection = [&](const QString& title,
                               GraphWidget*& gw,
                               const QColor& col,
                               const QString& unit,
                               const QString& label,
                               double lo, double hi){
        auto* box = new QGroupBox(title);
        box->setFixedHeight(90);
        auto* l = new QVBoxLayout(box);
        l->setContentsMargins(2,16,2,2);
        gw = new GraphWidget(GraphWidget::MiniPlot, this);
        gw->setMiniPlotColor(col);
        gw->setMiniPlotUnit(unit);
        gw->setMiniPlotLabel(label);
        gw->setMiniPlotRange(lo,hi);
        l->addWidget(gw);
        layout->addWidget(box);
    };

    addMiniSection("Température T1 (K)", m_miniTemp1,
                   QColor("#00e676"),"°C","T1",0,80);
    addMiniSection("Température T2 (J)", m_miniTemp2,
                   QColor("#ffaa00"),"°C","T2",0,80);
    addMiniSection("V Bus 48V",          m_miniV1,
                   QColor("#00d4ff"),"V","VBus",40,55);
    addMiniSection("V Alim 12V",         m_miniV2,
                   QColor("#89ddff"),"V","V12",10,15);
    addMiniSection("Courant I AMP1",     m_miniI1,
                   QColor("#ffaa00"),"A","I1",0,6);
    addMiniSection("Courant I AMP2",     m_miniI2,
                   QColor("#c792ea"),"A","I2",0,6);

    // Mesure Sv
    addTextSection("Mesure Sv",
        {{"Gîte","127.5°"},{"Site","32.0°"},
         {"Sv moyen","−18.4 dB"},{"Progression","35%"}},
        {"info","info","info","info"});

    layout->addStretch();
    scroll->setWidget(container);

    // Injecter dans la sidebar du .ui
    auto* sideLayout = qobject_cast<QVBoxLayout*>(ui->sidebar->layout());
    ui->sidebar->setStyleSheet(
        "background:#161b22;border-left:1px solid #30363d;");
    sideLayout->addWidget(scroll);
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
        QString("Connecté — %1 · NI PXIe-8301 · Fs 96 kHz")
        .arg(m_connConfig.chassis));
}

void MainWindow::onToggleVoie()
{
    auto* btn=qobject_cast<QPushButton*>(sender());
    if(!btn) return;
    QString key=btn->property("voieKey").toString();
    bool on=btn->isChecked();
    m_voies[key]=on;
    if(m_graphTime)   m_graphTime->setVoieActive(key,on);
    if(m_graphLevels) m_graphLevels->setVoieActive(key,on);
    if(m_graphSpec)   m_graphSpec->setVoieActive(key,on);
}

void MainWindow::updateMetrics()
{
    auto* rng=QRandomGenerator::global();
    double t1=24.0+rng->generateDouble()*0.8;
    double t2=47.0+rng->generateDouble()*2.0;
    double v1=48.0+rng->generateDouble()*0.6-0.3;
    double v2=12.0+rng->generateDouble()*0.3-0.15;
    double i1=3.5+rng->generateDouble()*0.8;
    double i2=0.3+rng->generateDouble()*0.2;

    if(m_miniTemp1) m_miniTemp1->pushMiniValue(t1);
    if(m_miniTemp2) m_miniTemp2->pushMiniValue(t2);
    if(m_miniV1)    m_miniV1->pushMiniValue(v1);
    if(m_miniV2)    m_miniV2->pushMiniValue(v2);
    if(m_miniI1)    m_miniI1->pushMiniValue(i1);
    if(m_miniI2)    m_miniI2->pushMiniValue(i2);
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
            m_configWindow->setStyleSheet(
                StyleManager::instance().configStyleSheet());
            m_configWindow->setAttribute(Qt::WA_DeleteOnClose,false);
            m_configWindow->resize(1100,750);
            m_configWindow->move(this->x()+60,this->y()+40);
            connect(m_configWindow,&QObject::destroyed,
                    this,[this](){m_configWindow=nullptr;});
        }
        m_configWindow->show();
        m_configWindow->raise();
        m_configWindow->activateWindow();
    } catch(...){
        qDebug()<<"Erreur ConfigWindow";
    }
}
