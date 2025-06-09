#include <QCoreApplication>
#include <QLoggingCategory>
#include <QTimer>
#include <QLegoHub>
#include <unistd.h>

int main(int argc, char **argv)
{
    QLoggingCategory::setFilterRules(QStringLiteral("qt.lego=true"));
    QCoreApplication app(argc, argv);

    QLegoHub hub(QStringLiteral("11:22"));
    hub.connect();

    QObject::connect(&sensors, &QLegoHub::accelerationChanged, [](const QVector3D &v) {
        // Accelerometer
        qDebug() << "Acceleration:" << v;
    });

    QTimer::singleShot(10000, &app, &QCoreApplication::quit);

    return app.exec();
}
