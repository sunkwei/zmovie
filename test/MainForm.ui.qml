import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import Player 1.0

Item {
    width: 1280
    height: 720

    Player {
        id: player1
        url: "rtsp://172.16.1.52/av0_0"

        x: 0
        y: 0
        width: 630
        height: 360

        MouseArea {
            anchors.fill: parent
            transformOrigin: Item.Center
            onClicked: player1.play()
        }
    }
}
