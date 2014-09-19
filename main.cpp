#include <QtGui/QGuiApplication>
#include <QQmlContext>
#include <QtQml>
#include "simplesettings.h"
#include "rruidata.h"
#include "qtquick2applicationviewer.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QCoreApplication::setApplicationName("rrui");
    app.setOrganizationName("RRUI");
    app.setOrganizationDomain("rrui.desbwa.org");
    app.setApplicationName("RR UI");

    QtQuick2ApplicationViewer viewer;
    SimpleSettings settings;
    qmlRegisterType<RRUIData>("RRUI", 1, 0, "RRUIData");
    viewer.rootContext()->setContextProperty("settings", &settings);
    viewer.setMainQmlFile(QStringLiteral("qml/RRUI/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
