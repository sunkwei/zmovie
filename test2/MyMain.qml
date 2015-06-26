import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtWebKit 3.0
import zonekey.qd 1.4

ApplicationWindow {
    title: qsTr("zonekey player testing")
    width: 1280
    height: 720
    visible: true

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: messageDialog.show(qsTr("Open action triggered"));
            }
            MenuItem {
                text: qsTr("E&xit")
                onTriggered: Qt.quit();
            }
        }
    }

    MyLogin {
        id: login;
        anchors.centerIn: parent;
        visible: false;
        onMyOk: {
            login.visible = false;
            main_page.visible = true;
        }
    }

    RowLayout {
        id: main_page;
        //anchors.fill: parent;
        spacing: 20;
        x: 0;
        y: 0;
        width: 1280;
        height: 720;

        MyPlayers {
            id: players;
            visible: false;
            anchors.left: parent.left;
            anchors.right: parent.right;
            height: 300;
        }

        WebView {
            id: browser;
            url: "http://www.baidu.com";
        }
    }


    // 此时加载完成???
    Component.onCompleted: {
        main_page.visible = true;
        login.visible = false;
    }

    MessageDialog {
        id: messageDialog
        title: qsTr("May I have your attention, please?")

        function show(caption) {
            messageDialog.text = caption;
            messageDialog.open();
        }
    }
}
