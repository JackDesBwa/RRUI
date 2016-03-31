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

import QtQuick 2.5
import QtQuick.Controls 1.0

ApplicationWindow {
    property string rrui_url: ""

    width: 800
    height: 600
    visible: true
    title: "RRUI"

    onRrui_urlChanged: loader.source = rrui_url

    Loader {
        id: loader
        anchors.fill: parent
        source: "selector.qml";

        Keys.onEscapePressed: source = "qrc:/selector.qml";

        onStatusChanged: {
            if (loader.status == Loader.Error)
                source = "selector.qml";
            else if (loader.status == Loader.Ready)
                console.log("Loading " + source);
        }
    }
}
