#ifndef HARDWAREMANAGER_H
#define HARDWAREMANAGER_H

#include <QObject>
#include <QString>

class HardwareManager : public QObject
{
    Q_OBJECT

public:
    explicit HardwareManager(QObject* parent = nullptr);

    bool connectToChassis(const QString& chassisName);
    void disconnectFromChassis();

    bool isConnected() const;

signals:
    void connectionChanged(bool connected);
    void logMessage(const QString& level, const QString& message);

private:
    bool m_connected = false;
    QString m_chassisName;
};

#endif
