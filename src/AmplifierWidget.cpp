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
#include <QFileDialog>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

AmplifierWidget::AmplifierWidget(int index, QWidget* parent)
    : QScrollArea(parent), m_index(index)
{
    setWidgetResizable(true);
    setFrameShape(QFrame::NoFrame);
    setupUi();
}

void AmplifierWidget::setupUi()
{
    auto* container = new QWidget(this);
    setWidget(container);
    auto* mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(10,10,10,10);
    mainLayout->setSpacing(8);

    setupSignalGroup(mainLayout);
    setupCarrierGroup(mainLayout);
    setupTempGroup(mainLayout);
    setupVoltGroup(mainLayout);

    mainLayout->addStretch();
}

void AmplifierWidget::setupSignalGroup(QVBoxLayout* layout)
{
    auto* grp = new QGroupBox(QString("Amp %1 — Signal & filtre").arg(m_index));
    auto* g = new QGridLayout(grp); g->setSpacing(6);

    m_editWav    = new QLineEdit("signal_38kHz.wav");
    m_editFilter = new QLineEdit("filtre_bpf.csv");

    auto* btnWav = new QPushButton("Charger WAV");
    btnWav->setProperty("cssClass","primary");
    connect(btnWav, &QPushButton::clicked, this, [this](){
        QString f = QFileDialog::getOpenFileName(this,"Signal WAV","","WAV (*.wav)");
        if (!f.isEmpty()) m_editWav->setText(f);
    });
    auto* btnFlt = new QPushButton("Charger filtre");
    btnFlt->setProperty("cssClass","primary");
    connect(btnFlt, &QPushButton::clicked, this, [this](){
        QString f = QFileDialog::getOpenFileName(this,"Filtre","","CSV (*.csv);;Tous (*.*)");
        if (!f.isEmpty()) m_editFilter->setText(f);
    });

    g->addWidget(new QLabel("Fichier WAV"), 0, 0);
    g->addWidget(m_editWav, 0, 1); g->addWidget(btnWav, 0, 2);
    g->addWidget(new QLabel("Filtre"),      1, 0);
    g->addWidget(m_editFilter, 1, 1); g->addWidget(btnFlt, 1, 2);

    layout->addWidget(grp);
}

void AmplifierWidget::setupCarrierGroup(QVBoxLayout* layout)
{
    auto* grp = new QGroupBox(QString("Amp %1 — Porteuse MLI").arg(m_index));
    auto* mainVL = new QVBoxLayout(grp);
    mainVL->setSpacing(6);

    // Grille paramètres
    auto* g = new QGridLayout();
    m_comboCarrier    = new QComboBox();
    m_comboCarrier->addItems({"Triangle","Dent de scie"});
    m_spinCarrierFreq = new QDoubleSpinBox();
    m_spinCarrierFreq->setRange(1,10e6);
    m_spinCarrierFreq->setValue(38400);
    m_spinCarrierFreq->setSuffix(" Hz");
    m_spinAmplitude   = new QDoubleSpinBox();
    m_spinAmplitude->setRange(0,2);
    m_spinAmplitude->setValue(1.0);
    m_spinAmplitude->setDecimals(2);
    m_spinOffset      = new QDoubleSpinBox();
    m_spinOffset->setRange(-1,1);
    m_spinOffset->setValue(0.0);
    m_spinOffset->setDecimals(2);

    g->addWidget(new QLabel("Type porteuse"),   0,0);
    g->addWidget(m_comboCarrier,                0,1);
    g->addWidget(new QLabel("Fréquence"),       0,2);
    g->addWidget(m_spinCarrierFreq,             0,3);
    g->addWidget(new QLabel("Amplitude (0→2)"), 1,0);
    g->addWidget(m_spinAmplitude,               1,1);
    g->addWidget(new QLabel("Offset (-1→+1)"),  1,2);
    g->addWidget(m_spinOffset,                  1,3);
    mainVL->addLayout(g);

    // Canvas MLI
    class MLICanvas : public QWidget {
    public:
        explicit MLICanvas(QWidget* p) : QWidget(p) {
            setMinimumHeight(60);
            setMaximumHeight(60);
        }
        void paintEvent(QPaintEvent*) override {
            QPainter p(this);
            p.setRenderHint(QPainter::Antialiasing);
            p.fillRect(rect(), QColor("#1f2937"));
            p.setPen(QPen(QColor("#00e676"), 1.2));
            QPainterPath path;
            int W=width(), H=height();
            for (int x=0; x<W; x++) {
                double ph=(double(x)/W)*18*M_PI;
                double cr=qSin(ph);
                double md=0.55*qSin(double(x)/W*2*M_PI*0.85);
                double ml=(cr>md)?1.0:-1.0;
                double y=H/2.0-ml*H*0.38;
                x==0 ? path.moveTo(x,y) : path.lineTo(x,y);
            }
            p.drawPath(path);
        }
    };
    auto* mliCanvas = new MLICanvas(this);

    // Boutons
    auto* btnMli = new QPushButton("Générer MLI");
    btnMli->setProperty("cssClass","primary");
    auto* btnAct = new QPushButton("Activer voie");
    btnAct->setProperty("cssClass","warning");
    btnAct->setCheckable(true);
    connect(btnAct, &QPushButton::toggled, this, [btnAct](bool on){
        btnAct->setText(on ? "Désactiver voie" : "Activer voie");
        btnAct->setProperty("cssClass", on ? "danger" : "warning");
        btnAct->style()->unpolish(btnAct);
        btnAct->style()->polish(btnAct);
    });
    connect(btnMli, &QPushButton::clicked, mliCanvas,
            QOverload<>::of(&QWidget::update));

    auto* btnRow = new QHBoxLayout();
    btnRow->addWidget(btnMli);
    btnRow->addWidget(btnAct);
    btnRow->addStretch();

    mainVL->addLayout(btnRow);
    mainVL->addWidget(mliCanvas);

    layout->addWidget(grp);
}
void AmplifierWidget::setupTempGroup(QVBoxLayout* layout)
{
    auto* grp = new QGroupBox("Température");
    auto* g = new QGridLayout(grp); g->setSpacing(6);

    m_spinTempProbes = new QSpinBox();       m_spinTempProbes->setRange(0,8); m_spinTempProbes->setValue(2);
    m_comboTempType  = new QComboBox();      m_comboTempType->addItems({"K","J","T","E","N","R","S","B"});
    m_spinTempFreq   = new QDoubleSpinBox(); m_spinTempFreq->setRange(0.01,1000); m_spinTempFreq->setValue(1); m_spinTempFreq->setSuffix(" Hz");
    m_editTempLabel  = new QLineEdit(QString("T_AMP%1").arg(m_index));

    g->addWidget(new QLabel("Nb sondes"),        0,0); g->addWidget(m_spinTempProbes, 0,1);
    g->addWidget(new QLabel("Type TC"),          0,2); g->addWidget(m_comboTempType,  0,3);
    g->addWidget(new QLabel("Freq. acq."),       1,0); g->addWidget(m_spinTempFreq,   1,1);
    g->addWidget(new QLabel("Label"),            1,2); g->addWidget(m_editTempLabel,  1,3);

    layout->addWidget(grp);
}

void AmplifierWidget::setupVoltGroup(QVBoxLayout* layout)
{
    auto* grp = new QGroupBox("Tension");
    auto* g = new QGridLayout(grp); g->setSpacing(6);

    m_spinVoltCh   = new QSpinBox();       m_spinVoltCh->setRange(0,16); m_spinVoltCh->setValue(2);
    m_comboVoltDiv = new QComboBox();      m_comboVoltDiv->addItems({"1/10","1/100","1/1000"});
    m_spinVoltFreq = new QDoubleSpinBox(); m_spinVoltFreq->setRange(0.01,1000); m_spinVoltFreq->setValue(10); m_spinVoltFreq->setSuffix(" Hz");
    m_editVoltLabel= new QLineEdit("V_BUS48");

    g->addWidget(new QLabel("Nb voies"),    0,0); g->addWidget(m_spinVoltCh,    0,1);
    g->addWidget(new QLabel("Division"),    0,2); g->addWidget(m_comboVoltDiv,  0,3);
    g->addWidget(new QLabel("Freq. acq."),  1,0); g->addWidget(m_spinVoltFreq,  1,1);
    g->addWidget(new QLabel("Label"),       1,2); g->addWidget(m_editVoltLabel, 1,3);

    layout->addWidget(grp);
}

AmplifierConfig AmplifierWidget::config() const
{
    return AmplifierConfig{
        m_index,
        m_editWav->text(), m_editFilter->text(),
        m_comboCarrier->currentText(),
        m_spinCarrierFreq->value(),
        m_spinAmplitude->value(), m_spinOffset->value(),
        m_spinTempProbes->value(), m_comboTempType->currentText(),
        m_spinTempFreq->value(),   m_editTempLabel->text(),
        m_spinVoltCh->value(),     m_comboVoltDiv->currentText(),
        m_spinVoltFreq->value(),   m_editVoltLabel->text(),
        0, "", 0, ""  // courant — à étendre
    };
}
