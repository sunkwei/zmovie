import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import zonekey.qd 1.1

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
            player1.url = "C:/Users/sunkw/kuaipan/videos/IMG_0418.MOV";
            player1.play();
        }
        button2.onClicked: player1.stop()
        button3.onClicked: {
            timer1.start()
        }

        Timer {
            id: timer1;
            interval: 1000
            repeat: true
            onTriggered: mf.timeChanged()
        }

        function timeChanged() {
            player1.rotation += 3;
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
