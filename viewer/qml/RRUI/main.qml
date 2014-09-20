import QtQuick 2.0
import RRUI 1.0

Rectangle {
    width: 360
    height: 360
    Text {
        text: qsTr("Hello World")
        anchors.centerIn: parent
    }
    Rectangle {
        id: a1;
        anchors.top: parent.top;
        anchors.left: parent.left;
        width: parent.width * mydata.test_bool;
        height: 20;
        color: 'blue';
    }
    Rectangle {
        anchors.top: a1.bottom;
        anchors.left: parent.left;
        width: parent.width * (1 + mydata.test_float) / 2;
        height: 20;
        color: 'red';
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }
    RRUIData {
        id: mydata

        Component.onCompleted: {
            this.set_messages_file("");
            this.connect("localhost", 8000);
        }
    }
}
