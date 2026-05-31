#pragma once
#include <QScrollArea>
#include "DataTypes.h"

class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;
class QComboBox;

class HydroWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit HydroWidget(int index, QWidget* parent = nullptr);
    HydrophoneConfig config() const;

private:
    void setupUi();

    int m_index;

    QSpinBox*       m_spinChannel   = nullptr;
    QDoubleSpinBox* m_spinGain      = nullptr;
    QSpinBox*       m_spinFs        = nullptr;
    QComboBox*      m_comboDynamic  = nullptr;
    QDoubleSpinBox* m_spinDistance  = nullptr;
    QDoubleSpinBox* m_spinDepth     = nullptr;
    QComboBox*      m_comboModel    = nullptr;
    QLineEdit*      m_editSensFile  = nullptr;
};
