#include "ToolsWidget.h"
#include "StyleManager.h"
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
#include <QtMath>

// ─── PolarWidget ──────────────────────────────────────────────────────────────
PolarWidget::PolarWidget(QWidget* parent) : QWidget(parent)
{
    setMinimumHeight(280);
    // Données de démo : patron de directivité simulé
    for(int i=0; i<360; i++){
        double a = qDegreesToRadians(double(i));
        // Lobe principal + lobes secondaires
        double lv = 0.85 * qAbs(qCos(a))
                  + 0.30 * qAbs(qCos(3*a))
                  + 0.15 * qAbs(qCos(5*a))
                  + 0.05 * (double(qrand()%100)/100.0);
        m_angles << double(i);
        m_levels << qBound(0.0, lv, 1.0);
    }
}

void PolarWidget::setData(const QVector<double>& angles,
                           const QVector<double>& levels)
{
    m_angles = angles;
    m_levels = levels;
    update();
}

void PolarWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    bool dark = StyleManager::instance().isDark();
    QColor bg   = dark ? QColor("#0d1117") : QColor("#f8fafc");
    QColor grid = dark ? QColor("#1f2937") : QColor("#e2e8f0");
    QColor txt  = dark ? QColor("#6e7681") : QColor("#718096");
    QColor acc  = QColor("#00d4ff");
    QColor acc2 = QColor("#00e676");

    p.fillRect(rect(), bg);

    int cx = width()/2, cy = height()/2;
    int R  = qMin(width(), height())/2 - 30;
    if(R<=0) return;

    // Cercles de référence
    p.setPen(QPen(grid, 0.8));
    for(int r=1; r<=4; r++){
        int rr = R*r/4;
        p.drawEllipse(QPoint(cx,cy), rr, rr);
    }

    // Axes angulaires (0°, 45°, 90°...)
    p.setPen(QPen(grid, 0.5, Qt::DashLine));
    for(int a=0; a<360; a+=45){
        double rad = qDegreesToRadians(double(a));
        p.drawLine(cx, cy,
                   int(cx + R*qCos(rad)),
                   int(cy - R*qSin(rad)));
    }

    // Labels angles
    p.setPen(txt);
    p.setFont(QFont("Courier New", 8));
    QStringList aLbls = {"0°","45°","90°","135°","180°","225°","270°","315°"};
    for(int i=0; i<8; i++){
        double rad = qDegreesToRadians(double(i*45));
        int lx = int(cx + (R+14)*qCos(rad)) - 8;
        int ly = int(cy - (R+14)*qSin(rad)) + 4;
        p.drawText(lx, ly, aLbls[i]);
    }

    // Labels dB (cercles)
    p.setPen(txt);
    for(int r=1; r<=4; r++){
        int rr=R*r/4;
        p.drawText(cx+2, cy-rr+10,
                   QString("%1dB").arg(-20+r*5));
    }

    // Titre
    p.setPen(acc);
    p.setFont(QFont("Courier New", 10, QFont::Bold));
    p.drawText(QRect(0,4,width(),18), Qt::AlignCenter,
               "Directivité Sv — Plan horizontal");

    if(m_angles.isEmpty()) return;

    // Tracé du patron de directivité
    QPainterPath path;
    bool first = true;
    for(int i=0; i<m_angles.size(); i++){
        double rad = qDegreesToRadians(m_angles[i]);
        double r   = m_levels[i] * R;
        double px2 = cx + r * qCos(rad);
        double py2 = cy - r * qSin(rad);
        if(first){ path.moveTo(px2,py2); first=false; }
        else path.lineTo(px2,py2);
    }
    path.closeSubpath();

    // Remplissage translucide
    QColor fill = acc; fill.setAlphaF(0.12);
    p.fillPath(path, fill);

    // Contour
    p.setPen(QPen(acc, 1.5));
    p.drawPath(path);

    // Point max (lobe principal)
    double maxLv=0; int maxIdx=0;
    for(int i=0;i<m_levels.size();i++)
        if(m_levels[i]>maxLv){ maxLv=m_levels[i]; maxIdx=i; }
    double mRad = qDegreesToRadians(m_angles[maxIdx]);
    double mr   = maxLv * R;
    p.setPen(Qt::NoPen); p.setBrush(acc2);
    p.drawEllipse(QPointF(cx+mr*qCos(mRad), cy-mr*qSin(mRad)), 4, 4);
    p.setPen(acc2);
    p.setFont(QFont("Courier New", 8, QFont::Bold));
    p.drawText(int(cx+mr*qCos(mRad))+6,
               int(cy-mr*qSin(mRad))-4,
               QString("%1°").arg(int(m_angles[maxIdx])));

    // Légende
    p.setPen(acc);  p.setFont(QFont("Courier New",8));
    p.drawText(8, height()-18, "— Sv mesuré");
    p.setPen(grid); p.drawText(8, height()-6, "-- ref. −20dB");
}

// ─── ToolsWidget ──────────────────────────────────────────────────────────────
ToolsWidget::ToolsWidget(QWidget* parent) : QScrollArea(parent)
{
    setWidgetResizable(true);
    setFrameShape(QFrame::NoFrame);
    setupUi();
}

void ToolsWidget::setupUi()
{
    auto* c=new QWidget(this); setWidget(c);
    auto* l=new QVBoxLayout(c);
    l->setContentsMargins(10,10,10,10); l->setSpacing(8);
    setupHydroMgmt(l);
    setupConfigMgmt(l);
    setupPolarDiagram(l);
    setupSvParams(l);
    setupSvFilter(l);
    l->addStretch();
}

void ToolsWidget::setupHydroMgmt(QVBoxLayout* l)
{
    auto* grp=new QGroupBox("Gestion hydrophones");
    auto* r=new QHBoxLayout(grp);
    auto* bc=new QPushButton("Créer");    bc->setProperty("cssClass","primary");
    auto* bm=new QPushButton("Modifier"); bm->setProperty("cssClass","warning");
    auto* bd=new QPushButton("Supprimer");bd->setProperty("cssClass","danger");
    r->addWidget(bc); r->addWidget(bm); r->addWidget(bd); r->addStretch();
    l->addWidget(grp);
}

void ToolsWidget::setupConfigMgmt(QVBoxLayout* l)
{
    auto* grp=new QGroupBox("Gestion configuration");
    auto* vl=new QVBoxLayout(grp);
    auto mk=[](const QString& t,const QString& c){
        auto* b=new QPushButton(t); b->setProperty("cssClass",c); return b;};
    auto* r1=new QHBoxLayout();
    r1->addWidget(mk("Sauvegarder","primary"));
    r1->addWidget(mk("Charger","warning"));
    r1->addWidget(mk("Supprimer","danger"));
    r1->addStretch();
    auto* r2=new QHBoxLayout();
    r2->addWidget(mk("Exporter","primary"));
    r2->addWidget(mk("Importer","warning"));
    r2->addStretch();
    vl->addLayout(r1); vl->addLayout(r2);
    l->addWidget(grp);
}

void ToolsWidget::setupPolarDiagram(QVBoxLayout* l)
{
    auto* grp = new QGroupBox("Diagramme de directivité Sv — Vue polaire");
    auto* vl  = new QVBoxLayout(grp);
    vl->setContentsMargins(6,18,6,8);

    m_polar = new PolarWidget(this);
    vl->addWidget(m_polar);

    // Boutons contrôle
    auto* br = new QHBoxLayout();
    auto* bStart = new QPushButton("▶  Démarrer mesure Sv");
    bStart->setProperty("cssClass","connect");
    bStart->setMinimumHeight(30);

    auto* bSim = new QPushButton("Simulation démo");
    bSim->setProperty("cssClass","primary");

    auto* bClear = new QPushButton("Effacer");
    bClear->setProperty("cssClass","warning");

    connect(bSim, &QPushButton::clicked, this, [this](){
        // Génère un nouveau patron de démo aléatoire
        QVector<double> angles, levels;
        for(int i=0;i<360;i++){
            double a=qDegreesToRadians(double(i));
            double lv=0.80*qAbs(qCos(a))
                     +0.25*qAbs(qCos(3*a+0.3))
                     +0.10*qAbs(qCos(7*a))
                     +0.04*(double(qrand()%100)/100.0);
            angles<<double(i);
            levels<<qBound(0.0,lv,1.0);
        }
        m_polar->setData(angles,levels);
    });

    connect(bClear, &QPushButton::clicked, this, [this](){
        m_polar->setData({},{});
    });

    br->addWidget(bStart); br->addWidget(bSim);
    br->addWidget(bClear); br->addStretch();
    vl->addLayout(br);

    // Infos mesure en cours
    auto* infoRow = new QHBoxLayout();
    auto* lblGite = new QLabel("Gîte :");
    lblGite->setStyleSheet("color:#8b949e;font-size:12px;");
    auto* valGite = new QLabel("—");
    valGite->setStyleSheet("color:#00d4ff;font-size:12px;font-weight:bold;");
    auto* lblSite = new QLabel("Site :");
    lblSite->setStyleSheet("color:#8b949e;font-size:12px;");
    auto* valSite = new QLabel("—");
    valSite->setStyleSheet("color:#00d4ff;font-size:12px;font-weight:bold;");
    auto* lblSv = new QLabel("Sv moy :");
    lblSv->setStyleSheet("color:#8b949e;font-size:12px;");
    auto* valSv = new QLabel("—");
    valSv->setStyleSheet("color:#00e676;font-size:12px;font-weight:bold;");
    infoRow->addWidget(lblGite); infoRow->addWidget(valGite);
    infoRow->addSpacing(16);
    infoRow->addWidget(lblSite); infoRow->addWidget(valSite);
    infoRow->addSpacing(16);
    infoRow->addWidget(lblSv);   infoRow->addWidget(valSv);
    infoRow->addStretch();
    vl->addLayout(infoRow);

    l->addWidget(grp);
}

void ToolsWidget::setupSvParams(QVBoxLayout* l)
{
    auto* grp=new QGroupBox("Mesure Sv — Paramètres");
    auto* g=new QGridLayout(grp); g->setSpacing(6);
    auto dbl=[](double lo,double hi,double v,const QString& s="",int d=1){
        auto* x=new QDoubleSpinBox(); x->setRange(lo,hi); x->setValue(v);
        x->setDecimals(d); if(!s.isEmpty()) x->setSuffix(s); return x;};
    m_spinListenCh=new QSpinBox(); m_spinListenCh->setRange(1,999); m_spinListenCh->setValue(1);
    m_comboHydro=new QComboBox(); m_comboHydro->addItems({"Reson TC4013","Brüel & Kjær 8103"});
    m_spinGain=dbl(0,60,20," dB");
    m_spinFs=new QSpinBox(); m_spinFs->setRange(1000,10000000);
    m_spinFs->setValue(96000); m_spinFs->setSuffix(" Hz");
    m_spinPMin=dbl(0,360,0," °");   m_spinPMax=dbl(0,360,360," °");
    m_spinRMin=dbl(0,90,0," °");    m_spinRMax=dbl(0,90,90," °");
    m_spinAngRes=dbl(0.1,10,1," °"); m_spinMeasT=dbl(1,10000,100," ms",0);
    m_spinFMin=dbl(1,1e6,20000," Hz",0); m_spinFMax=dbl(1,1e6,80000," Hz",0);
    m_spinFStep=dbl(1,1e5,1000," Hz",0);
    m_spinHD=dbl(0,1000,0.5," m",2);
    m_spinTD=dbl(0,1000,1.0," m",2);
    m_spinDist=dbl(0,1000,1.5," m",2);
    int r=0;
    auto a2=[&](const QString& l1,QWidget* w1,const QString& l2,QWidget* w2){
        g->addWidget(new QLabel(l1),r,0); g->addWidget(w1,r,1);
        g->addWidget(new QLabel(l2),r,2); g->addWidget(w2,r,3); r++;};
    auto aR=[&](const QString& lbl,QDoubleSpinBox* a,QDoubleSpinBox* b){
        auto* hl=new QHBoxLayout();
        hl->addWidget(a); hl->addWidget(new QLabel("→")); hl->addWidget(b);
        auto* w=new QWidget(); w->setLayout(hl);
        g->addWidget(new QLabel(lbl),r,0,1,1);
        g->addWidget(w,r,1,1,3); r++;};
    a2("Voie écoute",m_spinListenCh,"Hydrophone",m_comboHydro);
    a2("Gain",m_spinGain,"Freq. acq.",m_spinFs);
    aR("Plage gîte (°)",m_spinPMin,m_spinPMax);
    aR("Plage site (°)",m_spinRMin,m_spinRMax);
    a2("Résolution ang.",m_spinAngRes,"Temps mesure",m_spinMeasT);
    aR("Bande fréq. (Hz)",m_spinFMin,m_spinFMax);
    a2("Pas fréquentiel",m_spinFStep,"Immersion hydro",m_spinHD);
    a2("Immersion transd.",m_spinTD,"Distance h-t",m_spinDist);
    l->addWidget(grp);
}

void ToolsWidget::setupSvFilter(QVBoxLayout* l)
{
    auto* grp=new QGroupBox("Filtre Sv");
    auto* g=new QGridLayout(grp); g->setSpacing(6);
    auto* en=new QLineEdit("filtre_sv_38k");
    auto* bc=new QPushButton("Créer filtre"); bc->setProperty("cssClass","primary");
    auto* bs=new QPushButton("Sauvegarder");  bs->setProperty("cssClass","primary");
    g->addWidget(new QLabel("Nom filtre"),0,0); g->addWidget(en,0,1,1,2);
    g->addWidget(bc,1,0); g->addWidget(bs,1,1);
    l->addWidget(grp);
}

SvConfig ToolsWidget::svConfig() const {
    return {m_spinListenCh->value(),0,
            m_spinGain->value(),m_spinFs->value(),
            m_spinPMin->value(),m_spinPMax->value(),
            m_spinRMin->value(),m_spinRMax->value(),
            m_spinAngRes->value(),m_spinMeasT->value(),
            m_spinFMin->value(),m_spinFMax->value(),
            m_spinFStep->value(),
            m_spinHD->value(),m_spinTD->value(),m_spinDist->value()};
}




#pragma once
#include <QScrollArea>
#include <QVBoxLayout>
#include "DataTypes.h"

class QDoubleSpinBox;
class QSpinBox;
class QComboBox;

// ─── Widget diagramme polaire Sv ─────────────────────────────────────────────
class PolarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PolarWidget(QWidget* parent = nullptr);
    void setData(const QVector<double>& angles,
                 const QVector<double>& levels);
protected:
    void paintEvent(QPaintEvent*) override;
private:
    QVector<double> m_angles;
    QVector<double> m_levels;
};

// ─── Onglet Outils ────────────────────────────────────────────────────────────
class ToolsWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit ToolsWidget(QWidget* parent = nullptr);
    SvConfig svConfig() const;

private:
    void setupUi();
    void setupHydroMgmt(QVBoxLayout* l);
    void setupConfigMgmt(QVBoxLayout* l);
    void setupSvParams(QVBoxLayout* l);
    void setupSvFilter(QVBoxLayout* l);
    void setupPolarDiagram(QVBoxLayout* l);
    void generateDemoData();

    QSpinBox*       m_spinListenCh  = nullptr;
    QComboBox*      m_comboHydro    = nullptr;
    QDoubleSpinBox* m_spinGain      = nullptr;
    QSpinBox*       m_spinFs        = nullptr;
    QDoubleSpinBox* m_spinPMin      = nullptr;
    QDoubleSpinBox* m_spinPMax      = nullptr;
    QDoubleSpinBox* m_spinRMin      = nullptr;
    QDoubleSpinBox* m_spinRMax      = nullptr;
    QDoubleSpinBox* m_spinAngRes    = nullptr;
    QDoubleSpinBox* m_spinMeasT     = nullptr;
    QDoubleSpinBox* m_spinFMin      = nullptr;
    QDoubleSpinBox* m_spinFMax      = nullptr;
    QDoubleSpinBox* m_spinFStep     = nullptr;
    QDoubleSpinBox* m_spinHD        = nullptr;
    QDoubleSpinBox* m_spinTD        = nullptr;
    QDoubleSpinBox* m_spinDist      = nullptr;
    PolarWidget*    m_polar         = nullptr;
};


