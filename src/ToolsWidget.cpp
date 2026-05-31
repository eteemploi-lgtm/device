#include "ToolsWidget.h"
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
#include <QFileDialog>

ToolsWidget::ToolsWidget(QWidget* parent) : QScrollArea(parent)
{
    setWidgetResizable(true);
    setFrameShape(QFrame::NoFrame);
    setupUi();
}

void ToolsWidget::setupUi()
{
    auto* container = new QWidget(this);
    setWidget(container);
    auto* layout = new QVBoxLayout(container);
    layout->setContentsMargins(10,10,10,10);
    layout->setSpacing(8);

    setupHydroMgmt(layout);
    setupConfigMgmt(layout);
    setupSvParams(layout);
    setupSvFilter(layout);

    layout->addStretch();
}

void ToolsWidget::setupHydroMgmt(QVBoxLayout* l)
{
    auto* grp = new QGroupBox("Gestion hydrophones");
    auto* row = new QHBoxLayout(grp);
    auto* btnC = new QPushButton("Créer");     btnC->setProperty("cssClass","primary");
    auto* btnM = new QPushButton("Modifier");  btnM->setProperty("cssClass","warning");
    auto* btnD = new QPushButton("Supprimer"); btnD->setProperty("cssClass","danger");
    row->addWidget(btnC); row->addWidget(btnM); row->addWidget(btnD); row->addStretch();
    l->addWidget(grp);
}

void ToolsWidget::setupConfigMgmt(QVBoxLayout* l)
{
    auto* grp = new QGroupBox("Gestion configuration");
    auto* vl = new QVBoxLayout(grp);
    auto* row1 = new QHBoxLayout();
    auto* row2 = new QHBoxLayout();

    auto mkBtn = [](const QString& lbl, const QString& css) {
        auto* b = new QPushButton(lbl); b->setProperty("cssClass", css); return b;
    };
    row1->addWidget(mkBtn("Sauvegarder","primary"));
    row1->addWidget(mkBtn("Charger","warning"));
    row1->addWidget(mkBtn("Supprimer","danger"));
    row1->addStretch();
    row2->addWidget(mkBtn("Exporter","primary"));
    row2->addWidget(mkBtn("Importer","warning"));
    row2->addStretch();

    vl->addLayout(row1); vl->addLayout(row2);
    l->addWidget(grp);
}

void ToolsWidget::setupSvParams(QVBoxLayout* l)
{
    auto* grp = new QGroupBox("Mesure Sv — Paramètres");
    auto* g = new QGridLayout(grp); g->setSpacing(6);

    auto dbl = [](double lo, double hi, double val, const QString& suf="", int dec=1) {
        auto* s = new QDoubleSpinBox();
        s->setRange(lo,hi); s->setValue(val); s->setDecimals(dec);
        if (!suf.isEmpty()) s->setSuffix(suf);
        return s;
    };
    auto rng = [&](QDoubleSpinBox*& a, QDoubleSpinBox*& b,
                   double lo, double hi, double va, double vb, const QString& suf) {
        a = dbl(lo,hi,va,suf); b = dbl(lo,hi,vb,suf);
    };

    m_spinListenCh   = new QSpinBox(); m_spinListenCh->setRange(1,999); m_spinListenCh->setValue(1);
    m_comboHydro     = new QComboBox(); m_comboHydro->addItems({"Reson TC4013","Brüel & Kjær 8103"});
    m_spinGain       = dbl(0,60,20," dB");
    m_spinFs         = new QSpinBox(); m_spinFs->setRange(1000,10000000); m_spinFs->setValue(96000); m_spinFs->setSuffix(" Hz");
    rng(m_spinPitchMin,m_spinPitchMax,0,360, 0,360," °");
    rng(m_spinRollMin, m_spinRollMax, 0,90,  0,90, " °");
    m_spinAngRes     = dbl(0.1,10,1," °");
    m_spinMeasTime   = dbl(1,10000,100," ms",0);
    rng(m_spinFreqMin,m_spinFreqMax,1,1e6, 20000,80000," Hz");
    m_spinFreqStep   = dbl(1,1e5,1000," Hz",0);
    m_spinHydroDepth = dbl(0,1000,0.5," m",2);
    m_spinTransDepth = dbl(0,1000,1.0," m",2);
    m_spinDist       = dbl(0,1000,1.5," m",2);

    int r=0;
    auto add2 = [&](const QString& l1, QWidget* w1, const QString& l2, QWidget* w2){
        g->addWidget(new QLabel(l1),r,0); g->addWidget(w1,r,1);
        g->addWidget(new QLabel(l2),r,2); g->addWidget(w2,r,3); r++;
    };
    auto addRng = [&](const QString& lbl, QDoubleSpinBox* a, QDoubleSpinBox* b){
        auto* hl = new QHBoxLayout();
        hl->addWidget(a); hl->addWidget(new QLabel("→")); hl->addWidget(b);
        auto* w = new QWidget(); w->setLayout(hl);
        g->addWidget(new QLabel(lbl),r,0,1,1);
        g->addWidget(w,r,1,1,3); r++;
    };

    add2("Voie écoute",     m_spinListenCh, "Hydrophone",    m_comboHydro);
    add2("Gain",            m_spinGain,     "Freq. acq.",     m_spinFs);
    addRng("Plage gîte (°)",     m_spinPitchMin, m_spinPitchMax);
    addRng("Plage site (°)",     m_spinRollMin,  m_spinRollMax);
    add2("Résolution ang.", m_spinAngRes,   "Temps mesure",  m_spinMeasTime);
    addRng("Bande fréq. (Hz)",   m_spinFreqMin,  m_spinFreqMax);
    add2("Pas fréquentiel", m_spinFreqStep, "Immersion hydro",  m_spinHydroDepth);
    add2("Immersion transd.", m_spinTransDepth, "Distance h-t", m_spinDist);

    l->addWidget(grp);
}

void ToolsWidget::setupSvFilter(QVBoxLayout* l)
{
    auto* grp = new QGroupBox("Filtre Sv");
    auto* g = new QGridLayout(grp); g->setSpacing(6);

    auto* editName = new QLineEdit("filtre_sv_38k");
    auto* btnCreate = new QPushButton("Créer filtre");  btnCreate->setProperty("cssClass","primary");
    auto* btnSave   = new QPushButton("Sauvegarder");   btnSave->setProperty("cssClass","primary");

    g->addWidget(new QLabel("Nom filtre"), 0, 0);
    g->addWidget(editName, 0, 1, 1, 2);
    g->addWidget(btnCreate, 1, 0);
    g->addWidget(btnSave,   1, 1);

    l->addWidget(grp);
}

SvConfig ToolsWidget::svConfig() const
{
    return SvConfig{
        m_spinListenCh->value(), 0,
        m_spinGain->value(), m_spinFs->value(),
        m_spinPitchMin->value(), m_spinPitchMax->value(),
        m_spinRollMin->value(),  m_spinRollMax->value(),
        m_spinAngRes->value(),   m_spinMeasTime->value(),
        m_spinFreqMin->value(),  m_spinFreqMax->value(),
        m_spinFreqStep->value(),
        m_spinHydroDepth->value(), m_spinTransDepth->value(),
        m_spinDist->value()
    };
}
