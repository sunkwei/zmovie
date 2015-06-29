import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls 1.2
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

    // 搞个特效 ..
    PropertyAnimation {
        target: login_wnd
        property: "opacity";
        duration: 1000;
        from: 0; to: 1;
        easing.type: Easing.InOutQuad ;
        running: true
    }

    // 背景 ..
    Rectangle {
        color: "#000000"
        opacity: 0.6
    }

    // pop upd
    Rectangle {
        id: login_wnd
        width: 400
        height: 300
        color: "#f56767"
        anchors.centerIn: parent
        radius: 10

        // 三层..
        ColumnLayout {
            anchors.margins: 30
            spacing: 20

            Row {
                spacing: 18

                Label {
                    text: qsTr("input target ip");
                }

                TextInput {
                    id: target_ip
                    width: 200
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pointSize: 12
                    text: "127.0.0.1"
                    color: "#ffffff"
                }
            }

            Row {
                spacing: 18

                Label {
                    text: qsTr("choose mode");
                }

                ComboBox {
                    id: mode
                    width: 150
                    model: [qsTr("teacher"), qsTr("student"), qsTr("bd")];
                }
            }

            Row {
                Button {
                    text: "ok";
                    onClicked: {
                        var who_name = ["teacher", "student", "bd"];
                        who = who_name[mode.currentIndex];
                        ip = target_ip.text;

                        myOk();
                    }
                }
            }
        }
    }
}
