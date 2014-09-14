#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QCoreApplication::setApplicationName("rrui");
    app.setOrganizationName("RRUI");
    app.setOrganizationDomain("rrui.desbwa.org");
    app.setApplicationName("RR UI");

    QtQuick2ApplicationViewer viewer;
    viewer.setMainQmlFile(QStringLiteral("qml/RRUI/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
