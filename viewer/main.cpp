/*
    This file is part of RRUI.

    RRUI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RRUI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RRUI.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtGui/QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>
#include "simplesettings.h"
#include "rruidata.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QCoreApplication::setApplicationName("rrui");
    app.setOrganizationName("RRUI");
    app.setOrganizationDomain("rrui.desbwa.org");
    app.setApplicationName("RR UI");

    QQmlApplicationEngine engine;
    SimpleSettings settings;
    qmlRegisterType<RRUIData>("RRUI", 1, 0, "RRUIData");
    engine.rootContext()->setContextProperty("rrui_settings", &settings);
    engine.load(QUrl(QStringLiteral("qrc:/window.qml")));

    QString path = ".";
    if (argc == 2) path = argv[1];
    if (path.right(1) != "/") path += '/';
    path += "main.qml";
    QFileInfo qmlfile(path);
    if (qmlfile.exists()) {
        path = "file:/" + qmlfile.absoluteFilePath();
        engine.rootObjects()[0]->setProperty("rrui_url", path);
    }
    if (path.startsWith("http://") || path.startsWith("https://"))
        engine.rootObjects()[0]->setProperty("rrui_url", path);

    return app.exec();
}
