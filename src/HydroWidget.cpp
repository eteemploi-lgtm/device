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

HydroWidget::HydroWidget(int index, QWidget* parent)
    : QScrollArea(parent), m_index(index)
{
    setWidgetResizable(true);
    setFrameShape(QFrame::NoFrame);
    setupUi();
}

void HydroWidget::setupUi()
{
    auto* container = new QWidget(this);
    setWidget(container);
    auto* mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(10,10,10,10);
    mainLayout->setSpacing(8);

    // ── Voie & acquisition ──────────────────────────────────────────────────
    auto* grpVoie = new QGroupBox(QString("Hydro %1 — Voie & acquisition").arg(m_index));
    auto* gridVoie = new QGridLayout(grpVoie);
    gridVoie->setSpacing(6);

    auto addField = [&](QGridLayout* g, int row, int col,
                        const QString& lbl, QWidget* w) {
        g->addWidget(new QLabel(lbl), row, col*2);
        g->addWidget(w, row, col*2+1);
    };

    m_spinChannel  = new QSpinBox();     m_spinChannel->setRange(1,999); m_spinChannel->setValue(m_index);
    m_spinGain     = new QDoubleSpinBox(); m_spinGain->setRange(0,60); m_spinGain->setValue(20); m_spinGain->setSuffix(" dB");
    m_spinFs       = new QSpinBox();     m_spinFs->setRange(1000,10000000); m_spinFs->setValue(96000); m_spinFs->setSuffix(" Hz");
    m_comboDynamic = new QComboBox();    m_comboDynamic->addItems({"±1V","±2V","±5V","±10V"});
    m_spinDistance = new QDoubleSpinBox(); m_spinDistance->setRange(0,1000); m_spinDistance->setValue(1.0); m_spinDistance->setSuffix(" m"); m_spinDistance->setDecimals(2);
    m_spinDepth    = new QDoubleSpinBox(); m_spinDepth->setRange(0,1000);   m_spinDepth->setValue(0.5);   m_spinDepth->setSuffix(" m"); m_spinDepth->setDecimals(2);

    addField(gridVoie, 0, 0, "N° voie",                   m_spinChannel);
    addField(gridVoie, 0, 1, "Gain préampli",              m_spinGain);
    addField(gridVoie, 1, 0, "Fréquence échantillonnage",  m_spinFs);
    addField(gridVoie, 1, 1, "Dynamique",                  m_comboDynamic);
    addField(gridVoie, 2, 0, "Distance hydro-transducteur",m_spinDistance);
    addField(gridVoie, 2, 1, "Profondeur immersion",        m_spinDepth);

    mainLayout->addWidget(grpVoie);

    // ── Sélection hydrophone ────────────────────────────────────────────────
    auto* grpModel = new QGroupBox("Hydrophone — Sélection");
    auto* gridModel = new QGridLayout(grpModel);
    gridModel->setSpacing(6);

    m_comboModel = new QComboBox();
    m_comboModel->addItems({"Reson TC4013","Brüel & Kjær 8103","DolphinEar Pro","Hydrophone custom"});
    gridModel->addWidget(new QLabel("Modèle"), 0, 0);
    gridModel->addWidget(m_comboModel, 0, 1, 1, 3);

    m_editSensFile = new QLineEdit("sensib_TC4013.csv");
    auto* btnBrowse = new QPushButton("Importer...");
    btnBrowse->setProperty("cssClass","primary");
    connect(btnBrowse, &QPushButton::clicked, this, [this](){
        QString f = QFileDialog::getOpenFileName(this, "Fichier de sensibilité", "",
                    "CSV (*.csv);;Tous (*.*)");
        if (!f.isEmpty()) m_editSensFile->setText(f);
    });
    gridModel->addWidget(new QLabel("Fichier sensibilité"), 1, 0);
    gridModel->addWidget(m_editSensFile, 1, 1, 1, 2);
    gridModel->addWidget(btnBrowse, 1, 3);

    mainLayout->addWidget(grpModel);

    // ── Boutons CRUD ────────────────────────────────────────────────────────
    auto* btnRow = new QHBoxLayout();
    auto* btnCreate = new QPushButton("Créer");  btnCreate->setProperty("cssClass","primary");
    auto* btnEdit   = new QPushButton("Modifier"); btnEdit->setProperty("cssClass","warning");
    auto* btnDel    = new QPushButton("Supprimer"); btnDel->setProperty("cssClass","danger");
    btnRow->addWidget(btnCreate); btnRow->addWidget(btnEdit); btnRow->addWidget(btnDel);
    btnRow->addStretch();
    mainLayout->addLayout(btnRow);

    mainLayout->addStretch();
}

HydrophoneConfig HydroWidget::config() const
{
    return HydrophoneConfig{
        m_index,
        m_spinChannel->value(),
        m_spinGain->value(),
        m_spinFs->value(),
        m_comboDynamic->currentText().remove("±").remove("V").toDouble(),
        m_spinDistance->value(),
        m_spinDepth->value(),
        m_comboModel->currentText(),
        m_editSensFile->text()
    };
}
