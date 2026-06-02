#pragma once
#include <QMainWindow>
#include <QTabWidget>
#include <QSpinBox>
#include "DataTypes.h"

class HydroWidget;
class AmplifierWidget;
class ToolsWidget;

class ConfigWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ConfigWindow(QWidget* parent = nullptr);
    SystemConfig config() const { return m_config; }

signals:
    void configChanged(const SystemConfig& config);

private slots:
    void onValidate();

private:
    void setupUi();
    void buildTabs();
    void setupLeftPanel();

    QWidget*    m_leftPanel  = nullptr;
    QTabWidget* m_tabWidget  = nullptr;
    QSpinBox*   m_spinHydros = nullptr;
    QSpinBox*   m_spinAmplis = nullptr;

    SystemConfig m_config;
};
