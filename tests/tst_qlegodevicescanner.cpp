#include <QTest>
#include "tst_qlegodevicescanner.h"
#include "qlegodevicescanner.h"

void QLegoDeviceScannerTest::testInit()
{
    QLegoDeviceScanner test;

    QVERIFY(test.devicesFound() == 0);
    QVERIFY(test.scanning() == false);
}

QTEST_MAIN(QLegoDeviceScannerTest)
