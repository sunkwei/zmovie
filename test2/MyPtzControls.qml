import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
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
        spin_speed.value = 1;
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
                    source: control.pressed ? "images/button_pressed.png" : control.hovered ?
                                                  "images/button_hover.png" : "images/button_default.png";
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

    Component {
        id: touchStyle_txt

        TextFieldStyle {
            textColor: "white";
            background: Item {
                implicitHeight: 50
                implicitWidth: 320
                BorderImage {
                    source: "../images/textinput.png"
                    border.left: 8
                    border.right: 8
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
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
                myPanTiltMoving("up")
                ptz.up(spin_speed.value);
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
                ptz.left(spin_speed.value);
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
                ptz.right(spin_speed.value);
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
                ptz.down(spin_speed.value);
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

    // 右侧显示转动速度 ...
    Text {
        id: text_speed;
        x: 180
        y: 40
        color: "white"
        text: "速度:"
        font {
            pixelSize: 20
        }
    }

    SpinBox {
        id: spin_speed;
        maximumValue: 7;
        minimumValue: 1;
        x: text_speed.x + text_speed.width + 10;
        y: 40;
        width: 50;
        font {
            pixelSize: 20;
        }
    }

//    Text {
//        id: text_preset;
//        x: 180
//        y: 40
//        color: "white"
//        text: "预制位:"
//        font {
//            pixelSize: 20
//        }
//    }

//    TextField {
//        id: input_preset_id
//        text: "1"
//        x: text_preset.x + text_preset.width + 10;
//        y: 20;
//        width: 40;
//        style: touchStyle_txt;
//        validator: IntValidator { bottom: 1; top: 12 }
//        font {
//            pixelSize: 20
//        }
//    }

//    Button {
//        id: button_preset_save;
//        text: "保存";
//        x: 180;
//        y: 80;
//        style: touchStyle_button;
//        onClicked: {
//            ptz.preset_save(input_preset_id.text);
//        }
//    }

//    Button {
//        id: button_preset_call;
//        text: "调用";
//        x: 180;
//        y: 120;
//        style: touchStyle_button;
//        onClicked: {
//            ptz.preset_call(input_preset_id.text);
//        }
//    }

//    Button {
//        id: button_preset_clr;
//        text: "删除";
//        x: 180
//        y: 160
//        style: touchStyle_button;
//        onClicked: {
//            ptz.preset_clear(input_preset_id.text);
//        }
//    }
}
