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

import QtQuick 2.0
import QtQuick.Dialogs 1.0
import QtQuick.Controls 1.0

Item {
    id: root
    FileDialog {
        id: dirSelector
        title: qsTr("Open HMI")
        selectFolder: true
        folder: rrui_settings.get_string("rrui_qml_dir")
        onAccepted: url.text = dirSelector.fileUrl;
    }
    Column {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        TextField {
            id: url
            width: parent.width
            placeholderText: qsTr("HMI directory or URL")
            Component.onCompleted: text = rrui_settings.get_string("rrui_qml_dir")
        }
        Button {
            width: parent.width
            text: qsTr("Select with file dialog")
            onClicked: dirSelector.visible = true
        }
        Button {
            width: parent.width
            text: qsTr("Load")
            onClicked: {
                loader.source = url.text + "/main.qml";
                rrui_url = url.text;
                rrui_settings.set_string("rrui_qml_dir", url.text);
            }
        }
    }
    Button {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 10
        text: qsTr("Open new window")
        onClicked: Qt.createComponent("window.qml").createObject();
    }
}
