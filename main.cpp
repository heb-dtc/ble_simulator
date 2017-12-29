#include <iostream>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qloggingcategory.h>
#include "peripheral.h"

int main(int argc, char *argv[]) {
    std::cout << "Starting akolyt simulator" << std::endl;

    QLoggingCategory::setFilterRules(QStringLiteral("qt.bluetooth* = true"));
    QCoreApplication app(argc, argv);

    Peripheral* peripheral = new Peripheral("AkolytSimul4tor");
    peripheral->advertise();

    return app.exec();
}
