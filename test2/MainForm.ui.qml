import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import zonekey.qd 1.4
import QtWebKit 3.0
import QtQuick.Controls.Styles 1.3

Item {
    id: item1
    width: 1280
    height: 720

    property alias button2: button_stop
    property alias button1: button_player
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
    property alias det_factor_05: sliderHorizontal_factor_05
    property alias det_factor_00: sliderHorizontal_factor_00
    property alias ma_player2: player2_ma

    RowLayout {
        id: player_layout
        y: 0
        anchors.left: parent.left
        anchors.right: parent.right
        height: 275
        anchors.rightMargin: 0
        anchors.leftMargin: 0

        // 左侧按钮区 ...
        ColumnLayout {
            Button {
                id: button_player
                text: qsTr("play")
            }

            Button {
                id: button_stop
                text: qsTr("stop")
            }

            Button {
                id: save_calibration
                text: qsTr("save_cal")
                property int current_state
                current_state: 0
            }
        }

        // 探测源视频 ...
        Player {
            id: player1
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
            }
        }

        // 学生标定的 ...
        ColumnLayout {
            Label {
                text: qsTr("thres_dis")
            }

            Slider {
                id: sliderHorizontal_thres_dis
                stepSize: 2
                minimumValue: 10
                value: 20
                maximumValue: 60
                tickmarksEnabled: true
                updateValueWhileDragging: true;

                style: SliderStyle {
                    handle: Rectangle {
                        height: 15;
                        width: height*2
                        radius: width/2
                        color: "#fff"

                        Text {
                            anchors.fill: parent
                            id: value_slider_thres_dis
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignHCenter
                            text: sliderHorizontal_thres_dis.value
                        }
                    }

                    groove: Rectangle {
                        implicitHeight: 10;
                        implicitWidth: 100
                        radius: height/2
                        border.color: "#333"
                        color: "#222"
                        Rectangle {
                            height: parent.height
                            width: styleData.handlePosition
                            implicitHeight: 6
                            implicitWidth: 100
                            radius: height/2
                            color: "#555"
                        }
                    }
                }
            }

            Label {
                text: qsTr("thres_area")
            }

            Slider {
                id: sliderHorizontal_thres_area
                stepSize: 100
                minimumValue: 3000
                value: 5000
                maximumValue: 8000
                tickmarksEnabled: true
                updateValueWhileDragging: true

                style: SliderStyle {
                    handle: Rectangle {
                        height: 15;
                        width: height*2
                        radius: width/2
                        color: "#fff"

                        Text {
                            anchors.fill: parent
                            id: value_slider_thres_area
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignHCenter
                            text: sliderHorizontal_thres_area.value
                        }
                    }

                    groove: Rectangle {
                        implicitHeight: 10;
                        implicitWidth: 100
                        radius: height/2
                        border.color: "#333"
                        color: "#222"
                        Rectangle {
                            height: parent.height
                            width: styleData.handlePosition
                            implicitHeight: 6
                            implicitWidth: 100
                            radius: height/2
                            color: "#555"
                        }
                    }
                }
            }

            // 中间系数 ..
            Label {
                text: "factor_05";
            }

            Slider {
                id: sliderHorizontal_factor_05
                stepSize: 0.02
                minimumValue: 0.2
                value: 0.35
                maximumValue: 0.6
                tickmarksEnabled: true
                updateValueWhileDragging: true;

                style: SliderStyle {
                    handle: Rectangle {
                        height: 15;
                        width: height*2
                        radius: width/2
                        color: "#fff"

                        Text {
                            anchors.fill: parent
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignHCenter
                            text: sliderHorizontal_factor_05.value
                        }
                    }

                    groove: Rectangle {
                        implicitHeight: 10;
                        implicitWidth: 100
                        radius: height/2
                        border.color: "#333"
                        color: "#222"
                        Rectangle {
                            height: parent.height
                            width: styleData.handlePosition
                            implicitHeight: 6
                            implicitWidth: 100
                            radius: height/2
                            color: "#555"
                        }
                    }
                }
            }

            // 后排系数 ...
            Label {
                text: qsTr("factor_00");
            }

            Slider {
                id: sliderHorizontal_factor_00
                stepSize: 0.02
                minimumValue: 0.02
                value: 0.10
                maximumValue: 0.40
                tickmarksEnabled: true
                updateValueWhileDragging: true;

                style: SliderStyle {
                    handle: Rectangle {
                        height: 15;
                        width: height*2
                        radius: width/2
                        color: "#fff"

                        Text {
                            anchors.fill: parent
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignHCenter
                            text: sliderHorizontal_factor_00.value
                        }
                    }

                    groove: Rectangle {
                        implicitHeight: 10;
                        implicitWidth: 100
                        radius: height/2
                        border.color: "#333"
                        color: "#222"
                        Rectangle {
                            height: parent.height
                            width: styleData.handlePosition
                            implicitHeight: 6
                            implicitWidth: 100
                            radius: height/2
                            color: "#555"
                        }
                    }
                }
            }
        }

        // 跟踪源视频 ...
        Player {
            id: player2
            width: 480
            height: 270
            fillColor: "#ffff00"
            url: "rtsp://172.16.1.52/av0_0"
            cl_enabled: false

            MouseArea {
                id: player2_ma;
                anchors.fill: parent;
                hoverEnabled: true;
            }
        }

        // 云台控制部分...，从上到下分别为 up, left, right, down, tele, wide
        ColumnLayout {
            Button {
                id: button_ptz_up
                text: qsTr("up")
            }

            Button {
                id: button_ptz_left
                text: qsTr("left")
            }

            Button {
                id: button_ptz_right
                text: qsTr("right")
            }

            Button {
                id: button_ptz_down
                text: qsTr("down")
            }

            Button {
                id: button_ptz_zoom_tele
                text: qsTr("tele")
            }

            Button {
                id: button_ptz_zoom_wide
                text: qsTr("wide")
            }
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
}
