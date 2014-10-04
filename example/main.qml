import QtQuick 2.0
import RRUI 1.0

Rectangle {
    width: 360
    height: 360

    RRUIData {
        id: example

        Component.onCompleted: {
            set_messages_file("example.conf")
            connect("127.0.0.1", 8000)
        }
    }

    Rectangle {
        color: "#ff0000"
        x: 0
        y : 0
        width: parent.width / 2
        height: parent.height
        visible: example.test_bool
    }

    Rectangle {
        color: "#00ff00"
        x: parent.width / 2
        y : 0
        width: parent.width / 2
        height: (example.test_float + 1) / 2 * parent.height
    }
}

