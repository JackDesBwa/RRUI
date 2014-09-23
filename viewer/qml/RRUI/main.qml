import QtQuick 2.0
import RRUI 1.0

Rectangle {
    width: 360
    height: 360
    Text {
        text: mydata.connected_state == 2 ? "Connecté" : "Non connecté"
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
            this.set_messages_file("dirty_test/test_data.conf");
            this.connect("localhost", 8000);
        }
    }
}
