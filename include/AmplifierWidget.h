#pragma once
#include <QScrollArea>
#include <QVBoxLayout>
#include "DataTypes.h"

class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;
class QComboBox;

class AmplifierWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit AmplifierWidget(int index, QWidget* parent = nullptr);
    AmplifierConfig config() const;

private:
    void setupUi();
    void setupSignalGroup(QVBoxLayout* layout);
    void setupCarrierGroup(QVBoxLayout* layout);
    void setupTempGroup(QVBoxLayout* layout);
    void setupVoltGroup(QVBoxLayout* layout);

    int m_index;

    QLineEdit*      m_editWav         = nullptr;
    QLineEdit*      m_editFilter      = nullptr;
    QComboBox*      m_comboCarrier    = nullptr;
    QDoubleSpinBox* m_spinCarrierFreq = nullptr;
    QDoubleSpinBox* m_spinAmplitude   = nullptr;
    QDoubleSpinBox* m_spinOffset      = nullptr;
    QSpinBox*       m_spinTempProbes  = nullptr;
    QComboBox*      m_comboTempType   = nullptr;
    QDoubleSpinBox* m_spinTempFreq    = nullptr;
    QLineEdit*      m_editTempLabel   = nullptr;
    QSpinBox*       m_spinVoltCh      = nullptr;
    QComboBox*      m_comboVoltDiv    = nullptr;
    QDoubleSpinBox* m_spinVoltFreq    = nullptr;
    QLineEdit*      m_editVoltLabel   = nullptr;
};