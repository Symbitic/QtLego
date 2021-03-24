#ifndef QLEGOMOTOR_H
#define QLEGOMOTOR_H

#include "qlegoglobal.h"
#include "qlegoattacheddevice.h"
#include <QtCore/QObject>

QT_FORWARD_DECLARE_CLASS(QString)

QT_BEGIN_NAMESPACE

class Q_LEGO_EXPORT QLegoMotor : public QLegoAttachedDevice
{
    Q_OBJECT
    Q_PROPERTY(int power READ power WRITE setPower NOTIFY powerChanged)

public:
    explicit QLegoMotor(DeviceType deviceType, quint8 portId, QObject *parent = nullptr);

    int power() const;

    Q_INVOKABLE void stop();
    Q_INVOKABLE void brake();

public Q_SLOTS:
    void setPower(int power);

Q_SIGNALS:
    void powerChanged();

private:
    int m_power;
};

QT_END_NAMESPACE

#endif
