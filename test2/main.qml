import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import zonekey.qd 1.1
import QtQuick.Layouts 1.1
import QtWebKit 3.0

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

    MainForm {
        id: mf
        anchors.fill: parent

        button1.onClicked: {
            //player1.url = "C:/Users/sunkw/kuaipan/videos/IMG_0418.MOV";
            player1.play();
        }

        button2.onClicked: player1.stop()

        browser.onNavigationRequested: mf.onRequest;

        function onRequest(req)
        {
            browser.url = req.url
        }

        button_home.onClicked: {
            browser.url = "http://172.16.1.10"
        }

        button_refresh.onClicked: {
            browser.url = input_url.text
        }
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
