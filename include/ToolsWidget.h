#pragma once
#include <QScrollArea>
#include <QVBoxLayout>
#include "DataTypes.h"

class QDoubleSpinBox;
class QSpinBox;
class QComboBox;

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

    QSpinBox*       m_spinListenCh    = nullptr;
    QComboBox*      m_comboHydro      = nullptr;
    QDoubleSpinBox* m_spinGain        = nullptr;
    QSpinBox*       m_spinFs          = nullptr;
    QDoubleSpinBox* m_spinPitchMin    = nullptr;
    QDoubleSpinBox* m_spinPitchMax    = nullptr;
    QDoubleSpinBox* m_spinRollMin     = nullptr;
    QDoubleSpinBox* m_spinRollMax     = nullptr;
    QDoubleSpinBox* m_spinAngRes      = nullptr;
    QDoubleSpinBox* m_spinMeasTime    = nullptr;
    QDoubleSpinBox* m_spinFreqMin     = nullptr;
    QDoubleSpinBox* m_spinFreqMax     = nullptr;
    QDoubleSpinBox* m_spinFreqStep    = nullptr;
    QDoubleSpinBox* m_spinHydroDepth  = nullptr;
    QDoubleSpinBox* m_spinTransDepth  = nullptr;
    QDoubleSpinBox* m_spinDist        = nullptr;
};