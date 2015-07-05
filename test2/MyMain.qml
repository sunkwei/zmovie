import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import zonekey.qd 1.5

ApplicationWindow {
    title: qsTr("zonekey player testing")
    width: 1280
    height: 768
    visible: true

    KVConfig {
        id: kvc;
    }

    property string target_ip: "";
    property alias infos: infos;
    property alias mi_restart: mi_restart
    property alias mi_start: mi_start
    property alias mi_stop: mi_stop

    menuBar: MenuBar {
        UdpSocket {
            id: sock;
        }

        Menu {
            title: "操作";
            MenuItem {
                id: mi_restart;
                text: "重新启动跟踪程序";
                enabled: false;
                onTriggered: {
                    // 发送 restart 命令
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
                text: "退出"
                onTriggered: Qt.quit();
            }
        }
    }

    toolBar: BorderImage {
        border.bottom: 8
        source: "images/toolbar.png"
        width: parent.width
        height: 50

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
            color: backmouse.pressed ? "#222" : "transparent"
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
