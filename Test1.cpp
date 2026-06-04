#include "HydroWidget.h"
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
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QMessageBox>

HydroWidget::HydroWidget(int index, QWidget* parent)
    : QScrollArea(parent), m_index(index)
{ setWidgetResizable(true); setFrameShape(QFrame::NoFrame); setupUi(); }

void HydroWidget::setupUi()
{
    auto* c=new QWidget(this); setWidget(c);
    auto* ml=new QVBoxLayout(c);
    ml->setContentsMargins(10,10,10,10); ml->setSpacing(8);

    // ── Voie & acquisition ──
    auto* grpV=new QGroupBox(QString("Hydro %1 — Voie & acquisition").arg(m_index));
    auto* g=new QGridLayout(grpV); g->setSpacing(6);
    m_spinChannel=new QSpinBox(); m_spinChannel->setRange(1,999); m_spinChannel->setValue(m_index);
    m_spinGain=new QDoubleSpinBox(); m_spinGain->setRange(0,60); m_spinGain->setValue(20); m_spinGain->setSuffix(" dB");
    m_spinFs=new QSpinBox(); m_spinFs->setRange(1000,10000000); m_spinFs->setValue(96000); m_spinFs->setSuffix(" Hz");
    m_comboDyn=new QComboBox(); m_comboDyn->addItems({"±1V","±2V","±5V","±10V"});
    m_spinDist=new QDoubleSpinBox(); m_spinDist->setRange(0,1000); m_spinDist->setValue(1.0); m_spinDist->setSuffix(" m"); m_spinDist->setDecimals(2);
    m_spinDepth=new QDoubleSpinBox(); m_spinDepth->setRange(0,1000); m_spinDepth->setValue(0.5); m_spinDepth->setSuffix(" m"); m_spinDepth->setDecimals(2);
    g->addWidget(new QLabel("N° voie"),0,0);         g->addWidget(m_spinChannel,0,1);
    g->addWidget(new QLabel("Gain préampli"),0,2);   g->addWidget(m_spinGain,0,3);
    g->addWidget(new QLabel("Fréquence éch."),1,0);  g->addWidget(m_spinFs,1,1);
    g->addWidget(new QLabel("Dynamique"),1,2);        g->addWidget(m_comboDyn,1,3);
    g->addWidget(new QLabel("Distance h-t"),2,0);    g->addWidget(m_spinDist,2,1);
    g->addWidget(new QLabel("Profondeur"),2,2);       g->addWidget(m_spinDepth,2,3);
    ml->addWidget(grpV);

    // ── Hydrophone ──
    auto* grpM=new QGroupBox("Hydrophone — Sélection");
    auto* gm=new QGridLayout(grpM); gm->setSpacing(6);
    m_comboModel=new QComboBox();
    m_comboModel->addItems({"Reson TC4013","Brüel & Kjær 8103","Custom"});
    m_editSens=new QLineEdit("sensib_TC4013.csv");
    auto* btnB=new QPushButton("Importer..."); btnB->setProperty("cssClass","primary");
    connect(btnB,&QPushButton::clicked,this,[this](){
        QString f=QFileDialog::getOpenFileName(this,"Sensibilité","","CSV (*.csv);;Tous (*.*)");
        if(!f.isEmpty()) m_editSens->setText(f);
    });
    gm->addWidget(new QLabel("Modèle"),0,0);
    gm->addWidget(m_comboModel,0,1,1,3);
    gm->addWidget(new QLabel("Fichier sensibilité"),1,0);
    gm->addWidget(m_editSens,1,1,1,2);
    gm->addWidget(btnB,1,3);
    ml->addWidget(grpM);

    // ── Bouton Appliquer ──
    auto* btnApply=new QPushButton("✔  Appliquer les modifications");
    btnApply->setProperty("cssClass","connect");
    btnApply->setMinimumHeight(32);
    connect(btnApply,&QPushButton::clicked,this,[this](){
        emit hydroConfigChanged(config());
    });
    ml->addWidget(btnApply);

    // ── Boutons CRUD ──
    auto* br=new QHBoxLayout();
    auto* btnC=new QPushButton("Créer");   btnC->setProperty("cssClass","primary");
    auto* btnM=new QPushButton("Modifier");btnM->setProperty("cssClass","warning");
    auto* btnD=new QPushButton("Supprimer");btnD->setProperty("cssClass","danger");

    connect(btnC,&QPushButton::clicked,this,[this](){
        showHydroDialog(false);
    });
    connect(btnM,&QPushButton::clicked,this,[this](){
        showHydroDialog(true);
    });
    connect(btnD,&QPushButton::clicked,this,[this](){
        auto r=QMessageBox::question(this,"Supprimer",
            QString("Supprimer Hydro %1 ?").arg(m_index),
            QMessageBox::Yes|QMessageBox::No);
        if(r==QMessageBox::Yes)
            emit hydroDeleted(m_index);
    });

    br->addWidget(btnC); br->addWidget(btnM); br->addWidget(btnD);
    br->addStretch();
    ml->addLayout(br);
    ml->addStretch();
}

void HydroWidget::showHydroDialog(bool editMode)
{
    QDialog dlg(this);
    dlg.setWindowTitle(editMode ?
        QString("Modifier Hydro %1").arg(m_index) :
        QString("Créer Hydro %1").arg(m_index));
    dlg.setMinimumWidth(380);
    dlg.setStyleSheet(this->styleSheet());

    auto* form=new QFormLayout(&dlg);
    form->setSpacing(10);
    form->setContentsMargins(16,16,16,16);

    auto* spinCh=new QSpinBox(); spinCh->setRange(1,999);
    spinCh->setValue(editMode?m_spinChannel->value():m_index);
    auto* spinG=new QDoubleSpinBox(); spinG->setRange(0,60); spinG->setSuffix(" dB");
    spinG->setValue(editMode?m_spinGain->value():20.0);
    auto* spinFs=new QSpinBox(); spinFs->setRange(1000,10000000); spinFs->setSuffix(" Hz");
    spinFs->setValue(editMode?m_spinFs->value():96000);
    auto* comboDyn=new QComboBox(); comboDyn->addItems({"±1V","±2V","±5V","±10V"});
    if(editMode) comboDyn->setCurrentText(m_comboDyn->currentText());
    auto* comboMod=new QComboBox();
    comboMod->addItems({"Reson TC4013","Brüel & Kjær 8103","Custom"});
    if(editMode) comboMod->setCurrentText(m_comboModel->currentText());
    auto* spinDist=new QDoubleSpinBox(); spinDist->setRange(0,1000); spinDist->setSuffix(" m"); spinDist->setDecimals(2);
    spinDist->setValue(editMode?m_spinDist->value():1.0);
    auto* spinDep=new QDoubleSpinBox(); spinDep->setRange(0,1000); spinDep->setSuffix(" m"); spinDep->setDecimals(2);
    spinDep->setValue(editMode?m_spinDepth->value():0.5);

    form->addRow("N° voie",    spinCh);
    form->addRow("Gain",       spinG);
    form->addRow("Fs (Hz)",    spinFs);
    form->addRow("Dynamique",  comboDyn);
    form->addRow("Modèle",     comboMod);
    form->addRow("Distance h-t", spinDist);
    form->addRow("Profondeur", spinDep);

    auto* bbox=new QDialogButtonBox(
        QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    bbox->button(QDialogButtonBox::Ok)->setText(
        editMode?"Modifier":"Créer");
    bbox->button(QDialogButtonBox::Ok)
        ->setProperty("cssClass","connect");
    bbox->button(QDialogButtonBox::Cancel)
        ->setProperty("cssClass","warning");
    form->addRow(bbox);

    connect(bbox,&QDialogButtonBox::accepted,&dlg,&QDialog::accept);
    connect(bbox,&QDialogButtonBox::rejected,&dlg,&QDialog::reject);

    if(dlg.exec()==QDialog::Accepted){
        m_spinChannel->setValue(spinCh->value());
        m_spinGain->setValue(spinG->value());
        m_spinFs->setValue(spinFs->value());
        m_comboDyn->setCurrentText(comboDyn->currentText());
        m_comboModel->setCurrentText(comboMod->currentText());
        m_spinDist->setValue(spinDist->value());
        m_spinDepth->setValue(spinDep->value());
        emit hydroConfigChanged(config());
    }
}

HydrophoneConfig HydroWidget::config() const {
    return {m_index,m_spinChannel->value(),m_spinGain->value(),
            m_spinFs->value(),1.0,m_spinDist->value(),
            m_spinDepth->value(),m_comboModel->currentText(),
            m_editSens->text()};
}




#pragma once
#include <QScrollArea>
#include "DataTypes.h"
class QLineEdit; class QSpinBox; class QDoubleSpinBox; class QComboBox;

class HydroWidget : public QScrollArea {
    Q_OBJECT
public:
    explicit HydroWidget(int index, QWidget* parent = nullptr);
    HydrophoneConfig config() const;
signals:
    void hydroConfigChanged(const HydrophoneConfig& cfg);
    void hydroDeleted(int index);
private:
    void setupUi();
    void showHydroDialog(bool editMode);
    int m_index;
    QSpinBox*       m_spinChannel  = nullptr;
    QDoubleSpinBox* m_spinGain     = nullptr;
    QSpinBox*       m_spinFs       = nullptr;
    QComboBox*      m_comboDyn     = nullptr;
    QDoubleSpinBox* m_spinDist     = nullptr;
    QDoubleSpinBox* m_spinDepth    = nullptr;
    QComboBox*      m_comboModel   = nullptr;
    QLineEdit*      m_editSens     = nullptr;
};




#pragma once
#include <QScrollArea>
#include <QVBoxLayout>
#include "DataTypes.h"
class QLineEdit; class QSpinBox; class QDoubleSpinBox; class QComboBox;

class AmplifierWidget : public QScrollArea {
    Q_OBJECT
public:
    explicit AmplifierWidget(int index, QWidget* parent = nullptr);
    AmplifierConfig config() const;
signals:
    void ampConfigChanged(const AmplifierConfig& cfg);
    void ampDeleted(int index);
private:
    void setupUi();
    void setupSignalGroup(QVBoxLayout* l);
    void setupCarrierGroup(QVBoxLayout* l);
    void setupTempGroup(QVBoxLayout* l);
    void setupVoltGroup(QVBoxLayout* l);
    void showAmpDialog(bool editMode);
    int m_index;
    QLineEdit*      m_editWav=nullptr, *m_editFilter=nullptr;
    QComboBox*      m_comboCarrier=nullptr;
    QDoubleSpinBox* m_spinCFreq=nullptr, *m_spinAmp=nullptr, *m_spinOff=nullptr;
    QSpinBox*       m_spinTProbes=nullptr;
    QComboBox*      m_comboTType=nullptr;
    QDoubleSpinBox* m_spinTFreq=nullptr;
    QLineEdit*      m_editTLabel=nullptr;
    QSpinBox*       m_spinVCh=nullptr;
    QComboBox*      m_comboVDiv=nullptr;
    QDoubleSpinBox* m_spinVFreq=nullptr;
    QLineEdit*      m_editVLabel=nullptr;
};



    setupVoltGroup(ml);

    // ── Bouton Appliquer ──
    auto* btnApply=new QPushButton("✔  Appliquer les modifications");
    btnApply->setProperty("cssClass","connect");
    btnApply->setMinimumHeight(32);
    connect(btnApply,&QPushButton::clicked,this,[this](){
        emit ampConfigChanged(config());
    });
    ml->addWidget(btnApply);

    // ── Boutons CRUD ──
    auto* br=new QHBoxLayout();
    auto* btnC=new QPushButton("Créer");    btnC->setProperty("cssClass","primary");
    auto* btnM2=new QPushButton("Modifier");btnM2->setProperty("cssClass","warning");
    auto* btnD=new QPushButton("Supprimer");btnD->setProperty("cssClass","danger");
    connect(btnC, &QPushButton::clicked,this,[this](){ showAmpDialog(false); });
    connect(btnM2,&QPushButton::clicked,this,[this](){ showAmpDialog(true);  });
    connect(btnD, &QPushButton::clicked,this,[this](){
        auto r=QMessageBox::question(this,"Supprimer",
            QString("Supprimer Amp %1 ?").arg(m_index),
            QMessageBox::Yes|QMessageBox::No);
        if(r==QMessageBox::Yes) emit ampDeleted(m_index);
    });
    br->addWidget(btnC); br->addWidget(btnM2); br->addWidget(btnD);
    br->addStretch();
    ml->addLayout(br);
    ml->addStretch();





void AmplifierWidget::showAmpDialog(bool editMode)
{
    QDialog dlg(this);
    dlg.setWindowTitle(editMode?
        QString("Modifier Amp %1").arg(m_index):
        QString("Créer Amp %1").arg(m_index));
    dlg.setMinimumWidth(380);
    dlg.setStyleSheet(this->styleSheet());

    auto* form=new QFormLayout(&dlg);
    form->setSpacing(10); form->setContentsMargins(16,16,16,16);

    auto* editWav=new QLineEdit(editMode?m_editWav->text():"signal.wav");
    auto* comboC=new QComboBox(); comboC->addItems({"Triangle","Dent de scie"});
    if(editMode) comboC->setCurrentText(m_comboCarrier->currentText());
    auto* spinF=new QDoubleSpinBox(); spinF->setRange(1,10e6); spinF->setSuffix(" Hz");
    spinF->setValue(editMode?m_spinCFreq->value():38400);
    auto* spinA=new QDoubleSpinBox(); spinA->setRange(0,2); spinA->setDecimals(2);
    spinA->setValue(editMode?m_spinAmp->value():1.0);
    auto* spinO=new QDoubleSpinBox(); spinO->setRange(-1,1); spinO->setDecimals(2);
    spinO->setValue(editMode?m_spinOff->value():0.0);

    form->addRow("Fichier WAV",    editWav);
    form->addRow("Type porteuse",  comboC);
    form->addRow("Fréquence",      spinF);
    form->addRow("Amplitude (0→2)",spinA);
    form->addRow("Offset (−1→+1)", spinO);

    auto* bbox=new QDialogButtonBox(
        QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    bbox->button(QDialogButtonBox::Ok)->setText(editMode?"Modifier":"Créer");
    bbox->button(QDialogButtonBox::Ok)->setProperty("cssClass","connect");
    bbox->button(QDialogButtonBox::Cancel)->setProperty("cssClass","warning");
    form->addRow(bbox);

    connect(bbox,&QDialogButtonBox::accepted,&dlg,&QDialog::accept);
    connect(bbox,&QDialogButtonBox::rejected,&dlg,&QDialog::reject);

    if(dlg.exec()==QDialog::Accepted){
        m_editWav->setText(editWav->text());
        m_comboCarrier->setCurrentText(comboC->currentText());
        m_spinCFreq->setValue(spinF->value());
        m_spinAmp->setValue(spinA->value());
        m_spinOff->setValue(spinO->value());
        emit ampConfigChanged(config());
    }
}



#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QMessageBox>






void ConfigWindow::buildTabs()
{
    if(!m_tabWidget||!m_spinHydros||!m_spinAmplis) return;
    m_tabWidget->clear();
    int nh=m_spinHydros->value(), na=m_spinAmplis->value();

    for(int i=1;i<=nh;i++){
        auto* w=new HydroWidget(i,this);
        // Quand config hydro change → émettre configChanged
        connect(w,&HydroWidget::hydroConfigChanged,
                this,[this](const HydrophoneConfig& h){
            // Mettre à jour dans m_config
            for(auto& hc:m_config.hydrophones)
                if(hc.id==h.id){ hc=h; break; }
            emit configChanged(m_config);
        });
        connect(w,&HydroWidget::hydroDeleted,
                this,[this](int idx){
            m_config.hydrophones.removeIf(
                [idx](const HydrophoneConfig& h){ return h.id==idx; });
            emit configChanged(m_config);
            buildTabs();
        });
        m_tabWidget->addTab(w,QString("Hydro %1").arg(i));
    }

    for(int i=1;i<=na;i++){
        auto* w=new AmplifierWidget(i,this);
        connect(w,&AmplifierWidget::ampConfigChanged,
                this,[this](const AmplifierConfig& a){
            for(auto& ac:m_config.amplifiers)
                if(ac.id==a.id){ ac=a; break; }
            emit configChanged(m_config);
        });
        connect(w,&AmplifierWidget::ampDeleted,
                this,[this](int idx){
            m_config.amplifiers.removeIf(
                [idx](const AmplifierConfig& a){ return a.id==idx; });
            emit configChanged(m_config);
            buildTabs();
        });
        m_tabWidget->addTab(w,QString("Amp %1").arg(i));
    }

    m_tabWidget->addTab(new ToolsWidget(this),"⚙ Outils");
    if(m_tabWidget->count()>0) m_tabWidget->setCurrentIndex(0);
}



