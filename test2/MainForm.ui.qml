import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import zonekey.qd 1.1

Item {
    width: 1280
    height: 720

    property alias button3: button3
    property alias button2: button2
    property alias button1: button1
    property alias player1: player1

    RowLayout {
        anchors.verticalCenterOffset: 0
        anchors.horizontalCenterOffset: 0
        anchors.centerIn: parent

        Button {
            id: button1
            text: "play"
        }

        Button {
            id: button2
            text: qsTr("stop")
        }

        Button {
            id: button3
            text: qsTr("rotate")
        }

        Player {
            id: player1
            x: 300
            width: 480
            height: 270
            rotation: 20
            fillColor: "#ff0000"
            url: "rtsp://172.16.1.52/av0_0"
        }
    }
}
