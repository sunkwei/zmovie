import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import zonekey.qd 1.1
import QtWebKit 3.0

Item {
    id: item1
    width: 1280
    height: 720

    property alias button2: button2
    property alias button1: button1
    property alias player1: player1
    property alias browser: webView1
    property alias input_url: input_url
    property alias button_refresh: button_refresh
    property alias button_home: button_home

    RowLayout {
        id: player_layout
        anchors.left: parent.left
        anchors.right: parent.right
        height: 275

        Button {
            id: button1
            text: "play"
        }

        Button {
            id: button2
            text: qsTr("stop")
        }

        Player {
            id: player1
            x: 300
            width: 480
            height: 270
            rotation: 0
            fillColor: "#ff0000"
            url: "rtsp://172.16.1.52/av0_0"
        }
    }

    TextInput {
        id: input_url
        height: 34
        text: "http://www.baidu.com"
        anchors.rightMargin: 100
        anchors.leftMargin: -100
        font.family: "Courier"
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 15
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: player_layout.bottom
        focus: true
        persistentSelection: true
    }

    RowLayout {
        id: buttons
        height: 30
        anchors.top: input_url.bottom

        Button {
            id: button_refresh
            text: qsTr("refresh")
        }

        Button {
            id: button_home
            text: qsTr("home")
        }
    }

    WebView {
        id: webView1
        url: "http://172.16.1.10"
        anchors.top: buttons.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }
}
