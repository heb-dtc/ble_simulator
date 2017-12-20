#include <iostream>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qloggingcategory.h>
#include "akolyt.h"

int main(int argc, char *argv[]) {
    std::cout << "Starting akolyt simulator" << std::endl;

    QLoggingCategory::setFilterRules(QStringLiteral("qt.bluetooth* = true"));
    QCoreApplication app(argc, argv);

    Akolyt* akolyt = new Akolyt();
    akolyt->advertise();

    return app.exec();
}
