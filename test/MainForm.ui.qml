import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import Player 1.0

Item {
    width: 640
    height: 360

    Row {
        anchors.centerIn: parent
        spacing: 20

        Player {
            id: player1
            url: "rtsp://172.16.1.52/av0_0"

            x: 0
            y: 0
            width: 300
            height: 180

            MouseArea {
                anchors.fill: parent
                transformOrigin: Item.Center
                onClicked: player1.play()
            }
        }

        Player {
            id: player2
            url: "rtsp://172.16.1.53/av0_0"

            x: 300
            y: 0
            width: 300
            height: 160

            MouseArea{
                anchors.fill: parent
                transformOrigin: Item.Center
                onClicked: player2.play()
            }
        }
    }
}
