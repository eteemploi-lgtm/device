#include "ConfigWindow.h"
#include "HydroWidget.h"
#include "AmplifierWidget.h"
#include "ToolsWidget.h"
#include "StyleManager.h"

#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QGroupBox>
#include <QFrame>
#include <QMenuBar>

ConfigWindow::ConfigWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("ACOUSTIMETER — Configuration");
    setMinimumSize(900, 640);
    resize(1100, 750);
    setupUi();
}

void ConfigWindow::setupUi()
{
    menuBar()->addMenu("Fichier")->addAction("Sauvegarder JSON");
    menuBar()->addMenu("Outils");

    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* mainLayout = new QHBoxLayout(central);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,0,0,0);

    setupLeftPanel();
    mainLayout->addWidget(m_leftPanel);

    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabPosition(QTabWidget::North);
    mainLayout->addWidget(m_tabWidget, 1);

    buildTabs();
}

void ConfigWindow::setupLeftPanel()
{
    m_leftPanel = new QWidget(this);
    m_leftPanel->setFixedWidth(220);
    m_leftPanel->setObjectName("panelFrame");
    m_leftPanel->setStyleSheet(
        "QWidget#panelFrame { background:#161b22; border-right:1px solid #30363d; }");

    auto* layout = new QVBoxLayout(m_leftPanel);
    layout->setContentsMargins(12,14,12,14);
    layout->setSpacing(12);

    auto addSpinField = [&](const QString& label, QSpinBox*& spin, int defVal) {
        auto* lbl = new QLabel(label);
        lbl->setStyleSheet(
            "font-size:13px; color:#8b949e; font-weight:bold;");
        auto* row = new QHBoxLayout();
        auto* btnM = new QPushButton("−"); btnM->setFixedSize(26,26);
        spin = new QSpinBox();
        spin->setRange(0,999);
        spin->setValue(defVal);
        spin->setMinimumHeight(28);
        auto* btnP = new QPushButton("+"); btnP->setFixedSize(26,26);
        row->addWidget(btnM);
        row->addWidget(spin);
        row->addWidget(btnP);
        connect(btnM, &QPushButton::clicked,
                [spin](){ spin->setValue(spin->value()-1); });
        connect(btnP, &QPushButton::clicked,
                [spin](){ spin->setValue(spin->value()+1); });
        layout->addWidget(lbl);
        layout->addLayout(row);
    };

    addSpinField("Hydrophones",    m_spinHydros, 4);
    addSpinField("Amplificateurs", m_spinAmplis, 2);

    auto* btnValidate = new QPushButton("▶  VALIDER");
    btnValidate->setProperty("cssClass","connect");
    btnValidate->setMinimumHeight(34);
    connect(btnValidate, &QPushButton::clicked,
            this, &ConfigWindow::onValidate);
    layout->addWidget(btnValidate);

    auto* sep = new QFrame();
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color:#30363d;");
    layout->addWidget(sep);

    auto* lblCfg = new QLabel("Fichier config");
    lblCfg->setStyleSheet("font-size:13px; color:#8b949e; font-weight:bold;");
    layout->addWidget(lblCfg);

    auto* btnSave = new QPushButton("Sauvegarder JSON");
    btnSave->setProperty("cssClass","primary");
    btnSave->setMinimumHeight(30);
    auto* btnLoad = new QPushButton("Charger JSON");
    btnLoad->setProperty("cssClass","warning");
    btnLoad->setMinimumHeight(30);
    auto* btnExp  = new QPushButton("Exporter");
    btnExp->setProperty("cssClass","primary");
    btnExp->setMinimumHeight(30);

    layout->addWidget(btnSave);
    layout->addWidget(btnLoad);
    layout->addWidget(btnExp);
    layout->addStretch();
}

void ConfigWindow::onValidate()
{
    buildTabs();
}

void ConfigWindow::buildTabs()
{
    m_tabWidget->clear();

    int nh = m_spinHydros->value();
    int na = m_spinAmplis->value();

    for (int i = 1; i <= nh; i++) {
        auto* w = new HydroWidget(i, this);
        m_tabWidget->addTab(w, QString("Hydro %1").arg(i));
    }
    for (int i = 1; i <= na; i++) {
        auto* w = new AmplifierWidget(i, this);
        m_tabWidget->addTab(w, QString("Amp %1").arg(i));
    }

    auto* tools = new ToolsWidget(this);
    m_tabWidget->addTab(tools, "⚙ Outils");
}
