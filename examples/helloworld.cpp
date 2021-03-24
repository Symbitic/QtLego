#include "qlegodevicescanner.h"
#include "qlegomotor.h"
#include <QCoreApplication>
#include <QEventLoop>
#include <QTimer>
#include <QtDebug>
#include <QtEndian>
#include <QLoggingCategory>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QLoggingCategory::setFilterRules(QStringLiteral("lego.*=true"));

    auto scanner = new QLegoDeviceScanner();

    QObject::connect(scanner, &QLegoDeviceScanner::finished, [scanner]() {
        // Exit gracefully if no devices found.
        if (scanner->devicesFound() == 0) {
            qDebug() << "No devices found";
            qApp->exit(0);
        }
    });

    QObject::connect(scanner, &QLegoDeviceScanner::errorMessage, [scanner](const QString &msg) {
        qWarning() << msg;
        qApp->exit(1);
    });

    QObject::connect(scanner, &QLegoDeviceScanner::deviceFound, [=](QLegoDevice *device) {
        qDebug() << "  Address:" << device->address();
        qDebug() << "  Firmware:" << device->firmware();

        QObject::connect(device, &QLegoDevice::disconnected, [=]() {
            qDebug() << "  Disconnected";
            qApp->exit(0);
        });

        // Disconnect after 10 seconds.
        QTimer::singleShot(10000, device, &QLegoDevice::disconnect);

        // Run the right motor.
        QLegoMotor *rightMotor = device->waitForAttachedMotor("B");
        rightMotor->setPower(50);
        qDebug() << "  Waiting 3 seconds";
        device->wait(3000);
        qDebug() << "  done waiting";
        rightMotor->brake();
    });

    scanner->scan();

    return app.exec();
}
