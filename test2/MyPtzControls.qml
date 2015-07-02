import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import zonekey.qd 1.4

/** 云台控制模块，支持上下左右转动，变倍，预置位等 */
BorderImage {
    source: "images/toolbar.png"
    border.bottom: 5

    signal myPanTiltMoving(string cmd);
    signal myZoomChanging(string cmd);
    signal myPreset(string cmd, int idx);

    Component.onCompleted: {
        console.log("my size:" + width + "," + height);
    }

    property Ptz ptz;

    function reset(x, y, z)
    {
        ptz.set_pos(x, y, 36, 36);
        ptz.set_zoom(z);
    }

    Component {
        id: touchStyle_button
        ButtonStyle {
            panel: Item {
                implicitHeight: 30
                implicitWidth: 60
                BorderImage {
                    anchors.fill: parent
                    antialiasing: true
                    border.bottom: 8
                    border.top: 8
                    border.left: 8
                    border.right: 8
                    anchors.margins: control.pressed ? -4 : 0
                    source: control.pressed ? "../images/button_pressed.png" : "../images/button_default.png"
                    Text {
                        text: control.text
                        anchors.centerIn: parent
                        color: "white"
                        font.pixelSize: 23
                        renderType: Text.NativeRendering
                    }
                }
            }
        }
    }

    Button {
        id: button_up
        text: "上";
        style: touchStyle_button;
        x: 40;
        y: 40;
        onPressedChanged: {
            if (pressed) {
                myPanTiltMoving("up");
                ptz.up(1);
            }
            else {
                ptz.stop();
                myPanTiltMoving("stop");
            }
        }
    }

    Button {
        id: button_left
        text: "左";
        style: touchStyle_button
        x: 5
        y: 70
        onPressedChanged: {
            if (pressed) {
                myPanTiltMoving("left");
                ptz.left(1);
            }
            else {
                ptz.stop();
                myPanTiltMoving("stop");
            }
        }
    }

    Button {
        id: button_right
        text: "右";
        style: touchStyle_button
        x: 65
        y: 70
        onPressedChanged: {
            if (pressed) {
                myPanTiltMoving("right");
                ptz.right(1);
            }
            else {
                ptz.stop();
                myPanTiltMoving("stop");
            }
        }
    }


    Button {
        id: button_down
        text: "下";
        style: touchStyle_button
        x: 40
        y: 100
        onPressedChanged: {
            if (pressed) {
                myPanTiltMoving("down");
                ptz.down(1);
            }
            else {
                ptz.stop();
                myPanTiltMoving("stop");
            }
        }
    }

    Button {
        id: button_tele
        text: "推"
        style: touchStyle_button
        x: 5;
        y: 150;
        onPressedChanged: {
            if (pressed) {
                myZoomChanging("tele");
                ptz.zoom_tele(1);
            }
            else {
                ptz.zoom_stop();
                myZoomChanging("stop");
            }
        }
    }

    Button {
        id: button_wide
        text: "拉"
        style: touchStyle_button;
        x: 65;
        y: 150;
        onPressedChanged: {
            if (pressed) {
                myZoomChanging("wide");
                ptz.zoom_wide(1);
            }
            else {
                ptz.zoom_stop();
                myZoomChanging("stop");
            }
        }

    }

    /*
    ColumnLayout {
        spacing: 2

        Button {
            id: button_up
            text: qsTr("up");
            onPressedChanged: {
                if (pressed) {
                    myPanTiltMoving("up");
                    ptz.up(1);
                }
                else {
                    ptz.stop();
                    myPanTiltMoving("stop");
                }
            }
        }

        Button {
            id: button_left
            text: qsTr("left")
            onPressedChanged: {
                if (pressed) {
                    myPanTiltMoving("left");
                    ptz.left(1);
                }
                else {
                    ptz.stop();
                    myPanTiltMoving("stop");
                }
            }
        }

        Button {
            id: button_right
            text: qsTr("right");
            onPressedChanged: {
                if (pressed) {
                    ptz.right(1);
                    myPanTiltMoving("right");
                }
                else {
                    ptz.stop();
                    myPanTiltMoving("stop");
                }
            }
        }

        Button {
            id: button_down
            text: qsTr("down");
            onPressedChanged: {
                if (pressed) {
                    ptz.down(1);
                    myPanTiltMoving("down");
                }
                else {
                    ptz.stop();
                    myPanTiltMoving("stop");
                }
            }
        }

        Button {
            id: button_tele
            text: qsTr("tele");
            onPressedChanged: {
                if (pressed) {
                    ptz.zoom_tele(1);
                    myZoomChanging("tele");
                }
                else {
                    ptz.zoom_stop();
                    myZoomChanging("stop");
                }
            }
        }

        Button {
            id: button_wide
            text: qsTr("wide");
            onPressedChanged: {
                if (pressed) {
                    ptz.zoom_wide(1);
                    myZoomChanging("wide");
                }
                else {
                    ptz.zoom_stop();
                    myZoomChanging("stop");
                }
            }
        }

        ComboBox {
            id: combo_preset_select;
            width: 30
            model: ["1", "2", "3", "4", "5", "6"];
        }

        Button {
            id: button_preset_call;
            text: qsTr("preset call");
            onClicked: {
                var idx = combo_preset_select.currentText;
                myPreset("call", idx);
                ptz.preset_call(idx);
            }
        }

        Button {
            id: button_preset_save;
            text: qsTr("preset save");
            onClicked: {
                var idx = combo_preset_select.currentText;
                myPreset("save", idx);
                ptz.preset_save(idx);
            }
        }

        Button {
            id: button_preset_clear;
            text: qsTr("preset clear");
            onClicked: {
                var idx = combo_preset_select.currentText;
                myPreset("clear", idx);
                ptz.preset_clear(idx);
            }
        }
    }
    */
}
