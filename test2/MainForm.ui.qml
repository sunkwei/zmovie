import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import zonekey.qd 1.4
import QtWebKit 3.0

Item {
    id: item1
    width: 1280
    height: 720

    property alias button2: button2
    property alias button1: button1
    property alias player1: player1
    property alias player2: player2
    property alias browser: webView1
    property alias input_url: input_url
    property alias button_refresh: button_refresh
    property alias button_home: button_home
    property alias mouse_area: ma
    property alias info: info
    property alias save_calibration: save_calibration
    property alias ptz_left: button_ptz_left
    property alias ptz_up: button_ptz_up
    property alias ptz_right: button_ptz_right
    property alias ptz_down: button_ptz_down
    property alias ptz_zoom_tele: button_ptz_zoom_tele
    property alias ptz_zoom_wide: button_ptz_zoom_wide
    property alias det_thres_area: sliderHorizontal_thres_area
    property alias det_thres_dis: sliderHorizontal_thres_dis

    RowLayout {
        id: player_layout
        anchors.left: parent.left
        anchors.right: parent.right
        height: 275

        Button {
            id: button1
            text: "play"
            anchors.top: parent.top
            anchors.topMargin: 126
        }

        Button {
            id: button2
            text: qsTr("stop")
        }

        Player {
            id: player1
            //x: 300
            width: 480
            height: 270
            fillColor: "#ff0000"
            url: "rtsp://172.16.1.52/av0_0"
            cl_enabled: true

            MouseArea {
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                id: ma
                enabled: true
                anchors.fill: parent
                hoverEnabled: true

                Button {
                    id: button_ptz_up
                    x: 502
                    y: 95
                    width: 22
                    height: 23
                    text: qsTr("U")
                }

                Button {
                    id: button_ptz_zoom_tele
                    x: 476
                    y: 198
                    width: 26
                    height: 23
                    text: qsTr("T")
                }
            }
        }

        Player {
            id: player2
            width: 480
            height: 270
            fillColor: "#ffff00"
            url: "rtsp://172.16.1.52/av0_0"
            cl_enabled: false
        }
    }

    Text {
        id: info;
        anchors.top: player_layout.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 30
        text: "info"
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
        anchors.top: info.bottom
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

    Button {
        id: save_calibration
        x: 46
        y: 171
        text: qsTr("save_cal")
        property int current_state
        current_state: 0
    }

    Button {
        id: button_ptz_left
        x: 664
        y: 126
        width: 22
        height: 23
        text: qsTr("L")
    }

    Button {
        id: button_ptz_right
        x: 709
        y: 126
        width: 20
        height: 23
        text: qsTr("R")
    }

    Button {
        id: button_ptz_down
        x: 682
        y: 155
        width: 25
        height: 23
        text: qsTr("D")
    }

    Button {
        id: button_ptz_zoom_wide
        x: 697
        y: 200
        width: 23
        height: 23
        text: qsTr("W")
    }

    Slider {
        id: sliderHorizontal_thres_dis
        x: 8
        y: 201
        width: 160
        height: 22
        stepSize: 2
        minimumValue: 10
        value: 20
        maximumValue: 60
    }

    Slider {
        id: sliderHorizontal_thres_area
        x: 8
        y: 234
        width: 160
        height: 22
        stepSize: 10
        minimumValue: 3000
        value: 5000
        maximumValue: 8000
    }
}
