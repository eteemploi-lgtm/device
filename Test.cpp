#include "AmplifierWidget.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QPainter>
#include <QPainterPath>
#include <QFileDialog>
#include <QTimer>
#include <QtMath>

// ─── Canvas MLI dynamique ────────────────────────────────────────────────────
class MLICanvas : public QWidget {
    Q_OBJECT
public:
    explicit MLICanvas(QWidget* p) : QWidget(p) {
        setMinimumHeight(56); setMaximumHeight(56);
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));
    }
    void setParams(const QString& type, double freq, double amp, double offset) {
        m_type=type; m_freq=freq; m_amp=amp; m_offset=offset;
        update();
    }
    void setAnimating(bool on) {
        if(on) m_timer->start(33);
        else   m_timer->stop();
    }
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.fillRect(rect(), QColor("#1f2937"));
        int W=width(), H=height();
        // Porteuse
        p.setPen(QPen(QColor("#444c56"),0.8,Qt::DashLine));
        QPainterPath carrier;
        for(int x=0;x<W;x++){
            double ph=(double(x)/W)*m_freq/5000.0*2*M_PI + m_phase;
            double cv;
            if(m_type=="Triangle") cv=2.0/M_PI*qAsin(qSin(ph));
            else cv=(ph/M_PI - qFloor(ph/M_PI+0.5))*2.0; // Dent de scie
            cv=cv*m_amp+m_offset;
            double y=H/2.0-cv*H*0.35;
            x==0?carrier.moveTo(x,y):carrier.lineTo(x,y);
        }
        p.drawPath(carrier);
        // Signal modulé (sinusoïde lente)
        p.setPen(QPen(QColor("#00d4ff"),0.8,Qt::DotLine));
        QPainterPath sig;
        for(int x=0;x<W;x++){
            double sv=qSin(double(x)/W*2*M_PI*1.5)*0.8;
            double y=H/2.0-sv*H*0.35;
            x==0?sig.moveTo(x,y):sig.lineTo(x,y);
        }
        p.drawPath(sig);
        // MLI résultant
        p.setPen(QPen(QColor("#00e676"),1.4));
        QPainterPath mli;
        for(int x=0;x<W;x++){
            double ph=(double(x)/W)*m_freq/5000.0*2*M_PI+m_phase;
            double cv;
            if(m_type=="Triangle") cv=2.0/M_PI*qAsin(qSin(ph));
            else cv=(ph/M_PI-qFloor(ph/M_PI+0.5))*2.0;
            cv=cv*m_amp+m_offset;
            double sv=qSin(double(x)/W*2*M_PI*1.5)*0.8;
            double ml=(sv>cv)?1.0:-1.0;
            double y=H/2.0-ml*H*0.38;
            x==0?mli.moveTo(x,y):mli.lineTo(x,y);
        }
        p.drawPath(mli);
        // Légende
        p.setFont(QFont("Courier New",7));
        p.setPen(QColor("#444c56")); p.drawText(3,10,"porteuse");
        p.setPen(QColor("#00d4ff")); p.drawText(60,10,"signal");
        p.setPen(QColor("#00e676")); p.drawText(110,10,"MLI");
        m_phase += 0.04;
    }
private:
    QTimer* m_timer = nullptr;
    QString m_type   = "Triangle";
    double  m_freq   = 38400;
    double  m_amp    = 1.0;
    double  m_offset = 0.0;
    double  m_phase  = 0.0;
};
#include "AmplifierWidget.moc"

AmplifierWidget::AmplifierWidget(int index, QWidget* parent)
    : QScrollArea(parent), m_index(index)
{ setWidgetResizable(true); setFrameShape(QFrame::NoFrame); setupUi(); }

void AmplifierWidget::setupUi()
{
    auto* c=new QWidget(this); setWidget(c);
    auto* ml=new QVBoxLayout(c);
    ml->setContentsMargins(10,10,10,10); ml->setSpacing(8);
    setupSignalGroup(ml);
    setupCarrierGroup(ml);
    setupTempGroup(ml);
    setupVoltGroup(ml);
    ml->addStretch();
}

void AmplifierWidget::setupSignalGroup(QVBoxLayout* l)
{
    auto* grp=new QGroupBox(QString("Amp %1 — Signal & filtre").arg(m_index));
    auto* g=new QGridLayout(grp); g->setSpacing(6);
    m_editWav=new QLineEdit("signal_38kHz.wav");
    m_editFilter=new QLineEdit("filtre_bpf.csv");
    auto* bw=new QPushButton("Charger WAV"); bw->setProperty("cssClass","primary");
    auto* bf=new QPushButton("Charger filtre"); bf->setProperty("cssClass","primary");
    connect(bw,&QPushButton::clicked,this,[this](){
        QString f=QFileDialog::getOpenFileName(this,"Signal WAV","","WAV (*.wav)");
        if(!f.isEmpty()) m_editWav->setText(f);});
    connect(bf,&QPushButton::clicked,this,[this](){
        QString f=QFileDialog::getOpenFileName(this,"Filtre","","CSV (*.csv);;Tous (*.*)");
        if(!f.isEmpty()) m_editFilter->setText(f);});
    g->addWidget(new QLabel("Fichier WAV"),0,0); g->addWidget(m_editWav,0,1); g->addWidget(bw,0,2);
    g->addWidget(new QLabel("Filtre"),1,0); g->addWidget(m_editFilter,1,1); g->addWidget(bf,1,2);
    l->addWidget(grp);
}

void AmplifierWidget::setupCarrierGroup(QVBoxLayout* l)
{
    auto* grp=new QGroupBox(QString("Amp %1 — Porteuse MLI").arg(m_index));
    auto* ml2=new QVBoxLayout(grp); ml2->setSpacing(6);

    auto* g=new QGridLayout(); g->setSpacing(6);
    m_comboCarrier=new QComboBox(); m_comboCarrier->addItems({"Triangle","Dent de scie"});
    m_spinCFreq=new QDoubleSpinBox(); m_spinCFreq->setRange(1,10e6); m_spinCFreq->setValue(38400); m_spinCFreq->setSuffix(" Hz");
    m_spinAmp=new QDoubleSpinBox(); m_spinAmp->setRange(0,2); m_spinAmp->setValue(1.0); m_spinAmp->setDecimals(2);
    m_spinOff=new QDoubleSpinBox(); m_spinOff->setRange(-1,1); m_spinOff->setValue(0.0); m_spinOff->setDecimals(2);
    g->addWidget(new QLabel("Type porteuse"),0,0); g->addWidget(m_comboCarrier,0,1);
    g->addWidget(new QLabel("Fréquence"),0,2);     g->addWidget(m_spinCFreq,0,3);
    g->addWidget(new QLabel("Amplitude (0→2)"),1,0); g->addWidget(m_spinAmp,1,1);
    g->addWidget(new QLabel("Offset (−1→+1)"),1,2);  g->addWidget(m_spinOff,1,3);
    ml2->addLayout(g);

    // Canvas MLI dynamique
    auto* canvas = new MLICanvas(this);
    canvas->setParams(m_comboCarrier->currentText(),
                      m_spinCFreq->value(),
                      m_spinAmp->value(),
                      m_spinOff->value());

    // Mise à jour automatique quand les paramètres changent
    auto update = [this, canvas](){
        canvas->setParams(m_comboCarrier->currentText(),
                          m_spinCFreq->value(),
                          m_spinAmp->value(),
                          m_spinOff->value());
    };
    connect(m_comboCarrier, &QComboBox::currentTextChanged, this, update);
    connect(m_spinCFreq, &QDoubleSpinBox::valueChanged,     this, [update](double){ update(); });
    connect(m_spinAmp,   &QDoubleSpinBox::valueChanged,     this, [update](double){ update(); });
    connect(m_spinOff,   &QDoubleSpinBox::valueChanged,     this, [update](double){ update(); });

    // Boutons
    auto* br=new QHBoxLayout();
    auto* bm=new QPushButton("▶ Générer MLI"); bm->setProperty("cssClass","primary");
    auto* ba=new QPushButton("Activer voie");  ba->setProperty("cssClass","warning");
    ba->setCheckable(true);
    connect(bm, &QPushButton::clicked, this, [canvas](){
        canvas->setAnimating(true);
    });
    connect(ba, &QPushButton::toggled, this, [ba, canvas](bool on){
        ba->setText(on?"Désactiver voie":"Activer voie");
        ba->setProperty("cssClass", on?"danger":"warning");
        ba->style()->unpolish(ba); ba->style()->polish(ba);
        canvas->setAnimating(on);
    });
    br->addWidget(bm); br->addWidget(ba); br->addStretch();
    ml2->addLayout(br);
    ml2->addWidget(canvas);
    l->addWidget(grp);
}

void AmplifierWidget::setupTempGroup(QVBoxLayout* l)
{
    auto* grp=new QGroupBox("Température");
    auto* g=new QGridLayout(grp); g->setSpacing(6);
    m_spinTProbes=new QSpinBox(); m_spinTProbes->setRange(0,8); m_spinTProbes->setValue(2);
    m_comboTType=new QComboBox(); m_comboTType->addItems({"K","J","T","E","N","R","S","B"});
    m_spinTFreq=new QDoubleSpinBox(); m_spinTFreq->setRange(0.01,1000); m_spinTFreq->setValue(1); m_spinTFreq->setSuffix(" Hz");
    m_editTLabel=new QLineEdit(QString("T_AMP%1").arg(m_index));
    g->addWidget(new QLabel("Nb sondes"),0,0); g->addWidget(m_spinTProbes,0,1);
    g->addWidget(new QLabel("Type TC"),0,2);   g->addWidget(m_comboTType,0,3);
    g->addWidget(new QLabel("Freq. acq."),1,0); g->addWidget(m_spinTFreq,1,1);
    g->addWidget(new QLabel("Label"),1,2);      g->addWidget(m_editTLabel,1,3);
    l->addWidget(grp);
}

void AmplifierWidget::setupVoltGroup(QVBoxLayout* l)
{
    auto* grp=new QGroupBox("Tension");
    auto* g=new QGridLayout(grp); g->setSpacing(6);
    m_spinVCh=new QSpinBox(); m_spinVCh->setRange(0,16); m_spinVCh->setValue(2);
    m_comboVDiv=new QComboBox(); m_comboVDiv->addItems({"1/10","1/100","1/1000"});
    m_spinVFreq=new QDoubleSpinBox(); m_spinVFreq->setRange(0.01,1000); m_spinVFreq->setValue(10); m_spinVFreq->setSuffix(" Hz");
    m_editVLabel=new QLineEdit("V_BUS48");
    g->addWidget(new QLabel("Nb voies"),0,0);  g->addWidget(m_spinVCh,0,1);
    g->addWidget(new QLabel("Division"),0,2);  g->addWidget(m_comboVDiv,0,3);
    g->addWidget(new QLabel("Freq. acq."),1,0); g->addWidget(m_spinVFreq,1,1);
    g->addWidget(new QLabel("Label"),1,2);      g->addWidget(m_editVLabel,1,3);
    l->addWidget(grp);
}

AmplifierConfig AmplifierWidget::config() const {
    return {m_index,m_editWav->text(),m_editFilter->text(),
            m_comboCarrier->currentText(),m_spinCFreq->value(),
            m_spinAmp->value(),m_spinOff->value(),
            m_spinTProbes->value(),m_comboTType->currentText(),
            m_spinTFreq->value(),m_editTLabel->text(),
            m_spinVCh->value(),m_comboVDiv->currentText(),
            m_spinVFreq->value(),m_editVLabel->text(),
            0,"",0,""};
}




 #include "ConfigWindow.h"
#include "HydroWidget.h"
#include "AmplifierWidget.h"
#include "ToolsWidget.h"
#include "StyleManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QMenuBar>

ConfigWindow::ConfigWindow(QWidget* parent) : QMainWindow(parent)
{
    setWindowTitle("ACOUSTIMETER — Configuration");
    setMinimumSize(900,640); resize(1100,750);
    setupUi();
}

void ConfigWindow::setupUi()
{
    menuBar()->addMenu("Fichier")->addAction("Sauvegarder JSON");
    menuBar()->addMenu("Outils");
    auto* central=new QWidget(this); setCentralWidget(central);
    auto* mainLayout=new QHBoxLayout(central);
    mainLayout->setSpacing(0); mainLayout->setContentsMargins(0,0,0,0);
    setupLeftPanel();
    mainLayout->addWidget(m_leftPanel);
    m_tabWidget=new QTabWidget(this);
    m_tabWidget->setTabPosition(QTabWidget::North);
    mainLayout->addWidget(m_tabWidget,1);
    buildTabs();
}

void ConfigWindow::setupLeftPanel()
{
    m_leftPanel=new QWidget(this); m_leftPanel->setFixedWidth(220);
    m_leftPanel->setObjectName("panelFrame");
    m_leftPanel->setStyleSheet(
        "QWidget#panelFrame{background:#161b22;border-right:1px solid #30363d;}");
    auto* layout=new QVBoxLayout(m_leftPanel);
    layout->setContentsMargins(12,14,12,14); layout->setSpacing(12);

    auto addSpin=[&](const QString& lbl, QSpinBox*& spin, int def){
        auto* l=new QLabel(lbl);
        l->setStyleSheet("font-size:13px;color:#8b949e;font-weight:bold;");
        auto* row=new QHBoxLayout();
        auto* bm=new QPushButton("−"); bm->setFixedSize(26,26);
        spin=new QSpinBox(); spin->setRange(0,999);
        spin->setValue(def); spin->setMinimumHeight(28);
        auto* bp=new QPushButton("+"); bp->setFixedSize(26,26);
        row->addWidget(bm); row->addWidget(spin); row->addWidget(bp);
        connect(bm,&QPushButton::clicked,[spin](){spin->setValue(spin->value()-1);});
        connect(bp,&QPushButton::clicked,[spin](){spin->setValue(spin->value()+1);});
        layout->addWidget(l); layout->addLayout(row);
    };
    addSpin("Hydrophones",   m_spinHydros, 4);
    addSpin("Amplificateurs",m_spinAmplis, 2);

    auto* btnVal=new QPushButton("▶  VALIDER");
    btnVal->setProperty("cssClass","connect"); btnVal->setMinimumHeight(34);
    connect(btnVal,&QPushButton::clicked,this,&ConfigWindow::onValidate);
    layout->addWidget(btnVal);

    auto* sep=new QFrame(); sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color:#30363d;"); layout->addWidget(sep);

    auto* lc=new QLabel("Fichier config");
    lc->setStyleSheet("font-size:13px;color:#8b949e;font-weight:bold;");
    layout->addWidget(lc);
    auto* bs=new QPushButton("Sauvegarder JSON");
    bs->setProperty("cssClass","primary"); bs->setMinimumHeight(30);
    auto* bl=new QPushButton("Charger JSON");
    bl->setProperty("cssClass","warning"); bl->setMinimumHeight(30);
    auto* be=new QPushButton("Exporter");
    be->setProperty("cssClass","primary"); be->setMinimumHeight(30);
    layout->addWidget(bs); layout->addWidget(bl); layout->addWidget(be);
    layout->addStretch();
}

void ConfigWindow::onValidate()
{
    buildTabs();
    // Émettre le signal vers MainWindow avec le nouveau nombre de voies
    m_config.hydrophones.clear();
    m_config.amplifiers.clear();
    for(int i=1;i<=m_spinHydros->value();i++){
        HydrophoneConfig h; h.id=i; h.channel=i;
        m_config.hydrophones.append(h);
    }
    for(int i=1;i<=m_spinAmplis->value();i++){
        AmplifierConfig a; a.id=i;
        m_config.amplifiers.append(a);
    }
    emit configChanged(m_config);
}

void ConfigWindow::buildTabs()
{
    if(!m_tabWidget||!m_spinHydros||!m_spinAmplis) return;
    m_tabWidget->clear();
    int nh=m_spinHydros->value(), na=m_spinAmplis->value();
    for(int i=1;i<=nh;i++)
        m_tabWidget->addTab(new HydroWidget(i,this), QString("Hydro %1").arg(i));
    for(int i=1;i<=na;i++)
        m_tabWidget->addTab(new AmplifierWidget(i,this), QString("Amp %1").arg(i));
    m_tabWidget->addTab(new ToolsWidget(this),"⚙ Outils");
    if(m_tabWidget->count()>0) m_tabWidget->setCurrentIndex(0);
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

            // Connexion Config → Supervision
            connect(m_configWindow, &ConfigWindow::configChanged,
                    this, &MainWindow::onConfigChanged);

            connect(m_configWindow,&QObject::destroyed,
                    this,[this](){m_configWindow=nullptr;});
        }
        m_configWindow->show();
        m_configWindow->raise();
        m_configWindow->activateWindow();
    } catch(...){ qDebug()<<"Erreur ConfigWindow"; }
}




void MainWindow::onConfigChanged(const SystemConfig& cfg)
{
    // Mettre à jour la barre des voies selon le nouveau nombre de hydros/amplis
    auto* layout = qobject_cast<QHBoxLayout*>(ui->voieBar->layout());
    if(!layout) return;

    // Supprimer tous les widgets de la voie bar
    QLayoutItem* item;
    while((item=layout->takeAt(0))!=nullptr){
        if(item->widget()) item->widget()->deleteLater();
        delete item;
    }
    m_voieBtns.clear();

    // Reconstruire avec le nouveau nombre de voies
    struct VoieDef { QString key,label,color; };
    QList<VoieDef> hydros, amplis;
    QStringList hColors={"#00d4ff","#00e676","#ffaa00","#ff6b9d","#ff9944","#44ffaa"};
    QStringList aColors={"#c792ea","#89ddff","#ffcc44","#44ccff"};

    for(int i=0;i<cfg.hydrophones.size();i++)
        hydros.append({QString("h%1").arg(i+1),
                       QString("H%1").arg(i+1),
                       hColors[i%hColors.size()]});
    for(int i=0;i<cfg.amplifiers.size();i++)
        amplis.append({QString("a%1").arg(i+1),
                       QString("A%1").arg(i+1),
                       aColors[i%aColors.size()]});

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
        m_voies[d.key]=true;
        connect(btn,&QPushButton::toggled,this,&MainWindow::onToggleVoie);
        layout->addWidget(btn); m_voieBtns[d.key]=btn;
    };
    auto addSep=[&](){
        auto* s=new QFrame(); s->setFrameShape(QFrame::VLine);
        s->setStyleSheet("color:#30363d;"); layout->addWidget(s);
    };

    if(!hydros.isEmpty()){ addLbl("HYDROS :"); for(auto& d:hydros) addBtn(d); }
    if(!amplis.isEmpty()){ addSep(); addLbl("AMPLIS :"); for(auto& d:amplis) addBtn(d); }
    addSep();
    auto* info=new QLabel(
        "Fs <span style='color:#00d4ff;font-weight:bold'>96 kHz</span>"
        " &nbsp;|&nbsp; MLI "
        "<span style='color:#00e676;font-weight:bold'>38.4 kHz</span>");
    info->setTextFormat(Qt::RichText);
    info->setStyleSheet("font-size:13px;color:#8b949e;");
    layout->addWidget(info); layout->addStretch();

    appendLog("ok", QString("Configuration mise à jour — %1 hydros, %2 amplis")
              .arg(cfg.hydrophones.size()).arg(cfg.amplifiers.size()));
}

