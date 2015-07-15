import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import zonekey.qd 1.5

ApplicationWindow {
    title: qsTr("zonekey player testing")
    width: 1440
    height: 900
    visible: true

    KVConfig {
        id: kvc;
    }

    UdpSocket {
        id: sock;
    }

    property string target_ip: "";
    property alias infos: infos;
    property alias mi_restart: mi_restart
    property alias mi_start: mi_start
    property alias mi_stop: mi_stop

    menuBar: MenuBar {
        Menu {
            title: "操作";
            MenuItem {
                id: mi_restart;
                text: "重新启动跟踪程序";
                enabled: false;
                onTriggered: {
                    // 发送 restart 命令.
                    sock.send_cmd_restart();
                }
            }

            MenuItem {
                id: mi_start;
                text: "开始跟踪";
                enabled: false;
                onTriggered: {
                    sock.send_cmd_start();
                }
            }

            MenuItem {
                id: mi_stop;
                text: "停止跟踪";
                enabled: false;
                onTriggered: {
                    sock.send_cmd_stop();
                }
            }

            MenuItem {
                text: "退出";
                onTriggered: Qt.quit();
            }
        }
    }

    toolBar: BorderImage {
        border.bottom: 8
        source: "images/toolbar.png"
        width: parent.width
        height: 50
/*
        Rectangle {
            id: backButton
            width: opacity ? 30 : 0
            anchors.left: parent.left
            anchors.leftMargin: 20
            opacity: stackView.depth > 1 ? 1 : 0
            anchors.verticalCenter: parent.verticalCenter
            antialiasing: true
            height: 30
            radius: 4
            color: backmouse.pressed ? "#222" : "transparent";
            Behavior on opacity { NumberAnimation{} }
            Image {
                anchors.verticalCenter: parent.verticalCenter
                source: "images/navigation_previous_item.png"
            }
            MouseArea {
                id: backmouse
                anchors.fill: parent
                anchors.margins: -10
                onClicked: {
                    if (infos) {
                        infos.text = "";
                    }

                    stackView.pop()
                }
            }
        }

        Text {
            font.pixelSize: 21
            Behavior on x { NumberAnimation{ easing.type: Easing.OutCubic} }
            x: backButton.x + backButton.width + 20
            anchors.verticalCenter: parent.verticalCenter
            color: "white"
            text: qsTr("login")
        }
*/
        RowLayout {
            spacing: 20;
            anchors.centerIn: parent;
            opacity: stackView.depth > 1 ? 1 : 0

            Button {
                id: button_save_current;
                text: "保存当前，并返回";
                style: touchStyleButton;

                onClicked: {
                    // 保存当前配置，返回上一级 ...
                    if (stackView.depth > 1) {
                        stackView.currentItem.save();
                        stackView.pop();
                    }
                }
            }

            Button {
                id: button_cancel;
                text: "放弃修改，并返回";
                style: touchStyleButton;

                onClicked: {
                    if (stackView.depth > 1) {
                        stackView.currentItem.cancel();
                        stackView.pop();
                    }
                }
            }
        }

        Component {
            id: touchStyleButton
            ButtonStyle {
                panel: Item {
                    implicitHeight: 30
                    implicitWidth: 260
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
                            font.pixelSize: 19
                            renderType: Text.NativeRendering
                        }
                    }
                }
            }
        }

    }

    statusBar: BorderImage {
        border.bottom: 8
        source: "images/toolbar.png"
        width: parent.width
        height: 50

        Rectangle {
            id: s_backButton
            width: opacity ? 30 : 0
            anchors.left: parent.left
            anchors.leftMargin: 20
            opacity: 0;
            anchors.verticalCenter: parent.verticalCenter
            antialiasing: true
            height: 30
            radius: 4
            color: "transparent"
            Behavior on opacity { NumberAnimation{} }
            Image {
                anchors.verticalCenter: parent.verticalCenter
                source: "images/navigation_previous_item.png"
            }
        }

        Text {
            id: infos;
            font.pixelSize: 21
            Behavior on x { NumberAnimation{ easing.type: Easing.OutCubic} }
            x: s_backButton.x + s_backButton.width + 20
            anchors.verticalCenter: parent.verticalCenter
            color: "white"
            text: "";
        }
    }

    // 主窗口 ..
    StackView {
        id: stackView;
        anchors.fill: parent;

        delegate: StackViewDelegate {
            function transitionFinished(properties)
            {
                properties.exitItem.opacity = 1
            }

            pushTransition: StackViewTransition {
                PropertyAnimation {
                    target: enterItem
                    property: "opacity"
                    from: 0
                    to: 1
                }
                PropertyAnimation {
                    target: exitItem
                    property: "opacity"
                    from: 1
                    to: 0
                }
            }
        }

        function mydump(arr,level) {
            var dumped_text = "";
            if(!level) level = 0;

            var level_padding = "";
            for(var j=0;j<level+1;j++) level_padding += "    ";

            if(typeof(arr) == 'object') {
                for(var item in arr) {
                    var value = arr[item];

                    if(typeof(value) == 'object') {
                        dumped_text += level_padding + "'" + item + "' ...\n";
                        dumped_text += mydump(value,level+1);
                    } else {
                        dumped_text += level_padding + "'" + item + "' => \"" + value + "\"\n";
                    }
                }
            } else {
                dumped_text = "===>"+arr+"<===("+typeof(arr)+")";
            }
            return dumped_text;
        }

        initialItem: MyLogin {
            id: login;
            visible: false;
            anchors.centerIn: parent;
            onMyOk: {
                var ip = login.ip;
                var who = login.who;
                var fname = who + "_detect_trace.config";
                var config_url = "http://" + ip + ":8888/config/" + fname;

                target_ip = ip;
                mi_restart.enabled = true;
                mi_start.enabled = true;
                mi_stop.enabled = true;
                sock.targetHost = ip;
                sock.targetPort = 8642;

                kvc.fname = config_url; // 使用http接口 .
                if (kvc.reload() < 0) {
                    infos.text = "can't load config from:" + config_url;
                }
                else {
                    stackView.push({ item: "qrc:/MyMainPage.qml", properties:{ "kvc": kvc, "who": who }});
                }
            }
        }
    }
}
