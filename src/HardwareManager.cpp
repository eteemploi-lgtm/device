#include "HardwareManager.h"

HardwareManager::HardwareManager(QObject* parent)
    : QObject(parent)
{
}

bool HardwareManager::connectToChassis(const QString& chassisName)
{
    m_chassisName = chassisName;

    // Simulation pour l'instant
    m_connected = true;

    emit connectionChanged(m_connected);
    emit logMessage("ok", "Connexion simulée établie — " + m_chassisName);

    return m_connected;
}

void HardwareManager::disconnectFromChassis()
{
    m_connected = false;

    emit connectionChanged(m_connected);
    emit logMessage("warn", "Connexion interrompue — " + m_chassisName);
}

bool HardwareManager::isConnected() const
{
    return m_connected;
}
