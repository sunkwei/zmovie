import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import zonekey.qd 1.4

/** 显示登录界面:
        1. 输入目标主机的ip
        2. 选择配置模式，如教师，学生 ....
 */

Item {
    id: login_wnd0
    anchors.fill: parent

    property string ip
    property string who
    signal myOk()

    property real progress: 0
    SequentialAnimation on progress {
        loops: Animation.Infinite
        running: true
        NumberAnimation {
            from: 0
            to: 1
            duration: 3000
        }
        NumberAnimation {
            from: 1
            to: 0
            duration: 3000
        }
    }

    // pop upd
    BorderImage {
        id: login_wnd
        width: 450
        height: 300
        anchors.centerIn: parent
        border.bottom: 8
        source: "images/toolbar.png"

        // 三层..
        ColumnLayout {
            anchors.margins: 30
            spacing: 20

            Row {
                Layout.alignment: Qt.AlignHCenter;

                Label {
                    text: "输入跟踪机的ip";
                    font.pixelSize: 21
                    anchors.verticalCenter: parent.verticalCenter
                    color: "white";
                }

                TextField {
                    id: target_ip
                    width: 240
                    style: touchStyle_txt;
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pointSize: 19
                    text: "172.16.1.111"
                }
            }

            Button {
                text: "教师";
                style: touchStyle;
                Layout.alignment: Qt.AlignHCenter;

                onClicked: {
                    who = "teacher";
                    ip = target_ip.text;
                    myOk();
                }
            }

            Button {
                text: "学生";
                style: touchStyle;
                Layout.alignment: Qt.AlignHCenter;

                onClicked: {
                    who = "student";
                    ip = target_ip.text;
                    myOk();
                }
            }

            Button {
                text: "板书";
                style: touchStyle;
                Layout.alignment: Qt.AlignHCenter;

                onClicked: {
                    who = "bd";
                    ip = target_ip.text;
                    myOk();
                }
            }

            Component {
                id: touchStyle
                ButtonStyle {
                    panel: Item {
                        implicitHeight: 50
                        implicitWidth: 320
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
        }
    }
}
