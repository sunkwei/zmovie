import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import zonekey.qd 1.3
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

    KVConfig {
        id: kvc;
        fname: "student_detect_trace.config";
    }

    MainForm {
        id: mf
        anchors.fill: parent

        button1.onClicked: {
            player1.url = kvc.get("video_source");
            messageDialog.show("try to open source:" + player1.url);
            player1.play();
            player1.cl_pop_point()
        }

        button2.onClicked: player1.stop()

        browser.onNavigationRequested: {
            browser.url = req.url
        }

        button_home.onClicked: {
            browser.url = "http://172.16.1.111:8888/config/copy_card0.config/index"
        }

        button_refresh.onClicked: {
            browser.url = input_url.text
        }

        mouse_area.onReleased: {
            if (mouse.button === Qt.LeftButton) {
                player1.cl_push_point(mouse.x, mouse.y);
                info.text = "append points (" + mouse.x + ", " + mouse.y + ")";
            }
            else if (mouse.button === Qt.RightButton) {
                info.text = "right button released";

                player1.cl_pop_point();
            }
        }

        save_calibration.onClicked: {
            if (save_calibration.current_state == 0) {
                player1.cl_save();
                save_calibration.text = qsTr("enable_cal");
                save_calibration.current_state = 1;
                player1.cl_enabled = false;
            }
            else {
                save_calibration.text = qsTr("save_cal");
                save_calibration.current_state = 0;
                player1.cl_enabled = true;
                //player1.cl_remove_all_points();
            }
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
