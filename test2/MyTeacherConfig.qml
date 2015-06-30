import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import zonekey.qd 1.4
import QtQuick.Controls.Styles 1.2

/** 教师标定属性 ... */

Item {
    property KVConfig kvc;          // 全局配置项 ...
    id: sv;

    ColumnLayout {
        id: teachers_config;

        // 云台服务 ..
        MyKVPair {
            id: id_kv_ptz_serial_name;
            key: "ptz_serial_name";
            value: kvc.get(key);
            desc: qsTr("input ptz url");
            Layout.fillWidth: true;
            Layout.preferredHeight: 30;

            onValueChanged: {
                console.log(key + " changed to " + value);
                kvc.set(key, value);
            }
        }

        MyKVPair {
            id: id_kv_ptz_addr;
            key: "ptz_addr";
            value: kvc.get("ptz_addr");
            desc: qsTr("input ptz addr");
            Layout.fillWidth: true;
            Layout.preferredHeight: 30;

            onValueChanged: {
                console.log(key + " changed to " + value);
                kvc.set(key, value);
            }
        }

        // 探测源 url
        MyKVPair {
            id: id_kv_video_source;
            key: "video_source";
            value: kvc.get(key);
            desc: qsTr("video source");
            Layout.fillWidth: true;
            Layout.preferredHeight: 30;

            onValueChanged: {
                console.log(key + " changed to " + value);
                kvc.set(key, value);
            }
        }

        // 跟踪源 url
        MyKVPair {
            id: id_kv_video_source_tracing;
            key: "video_source_tracing";
            value: kvc.get(key);
            desc: qsTr("video source tracing");
            Layout.fillWidth: true;
            Layout.preferredHeight: 30;

            onValueChanged: {
                console.log(key + " changed to " + value);
                kvc.set(key, value);
            }
        }

        // 保存配置，并且返回 ..
        Button {
            text: "save & back";
            style: touchStyle;

            onClicked: {
                focus = true;   // 这样能强制使 ...

                kvc.save();

                if (stackView) {
                    stackView.pop();
                }
            }
        }

        // 放弃保存
        Button {
            text: "cancel";
            style: touchStyle;

            onClicked: {
                if (stackView) {
                    stackView.pop();
                }
                kvc.reload()
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
    }

    Component.onCompleted: {
        if (infos) {
            infos.text = "teacher calibration";
        }
    }
}
