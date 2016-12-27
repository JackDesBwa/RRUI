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

    Loader {
        id: loader
        anchors.fill: parent
        source: rrui_url ? rrui_url : "qrc:/selector.qml";

        Keys.onEscapePressed: rrui_url = "";

        onStatusChanged: {
            if (loader.status == Loader.Error) {
                rrui_url = "";
                console.log("Cannot load " + source);
            } else if (loader.status == Loader.Ready) {
                console.log("Loaded " + source);
            }
        }
    }
}
