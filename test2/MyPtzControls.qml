import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

/** 云台控制模块，支持上下左右转动，变倍，预置位等 */
Item {
    width: 100
    height: 270

    signal myPanTiltMoving();
    signal myZoomChanging();
    signal myPreset();

    ColumnLayout {
        spacing: 2

        Button {
            id: button_up
            text: qsTr("up");
            onPressedChanged: {
                if (pressed) {
                    myPanTiltMoving("up");
                }
                else {
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
                }
                else {
                    myPanTiltMoving("stop");
                }
            }
        }

        Button {
            id: button_right
            text: qsTr("right");
            onPressedChanged: {
                if (pressed) {
                    myPanTiltMoving("right");
                }
                else {
                    myPanTiltMoving("stop");
                }
            }
        }

        Button {
            id: button_down
            text: qsTr("down");
            onPressedChanged: {
                if (pressed) {
                    myPanTiltMoving("down");
                }
                else {
                    myPanTiltMoving("stop");
                }
            }
        }

        Button {
            id: button_tele
            text: qsTr("tele");
            onPressedChanged: {
                if (pressed) {
                    myZoomChanging("tele");
                }
                else {
                    myZoomChanging("stop");
                }
            }
        }

        Button {
            id: button_wide
            text: qsTr("wide");
            onPressedChanged: {
                if (pressed) {
                    myZoomChanging("wide");
                }
                else {
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
            }
        }

        Button {
            id: button_preset_save;
            text: qsTr("preset save");
            onClicked: {
                var idx = combo_preset_select.currentText;
                myPreset("save", idx);
            }
        }

        Button {
            id: button_preset_clear;
            text: qsTr("preset clear");
            onClicked: {
                var idx = combo_preset_select.currentText;
                myPreset("clear", idx);
            }
        }
    }
}
