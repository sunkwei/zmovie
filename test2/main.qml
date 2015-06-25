import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import zonekey.qd 1.4
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

    Ptz {
        id: ptz
    }

    MainForm {
        id: mf
        anchors.fill: parent

        button1.onClicked: {
            player1.url = kvc.get("video_source");
            player1.det_enabled = true;
            player1.play();

            player2.url = kvc.get("video_source_tracing");
            player2.play();
        }

        button2.onClicked: player1.stop()

        browser.onNavigationRequested: {
            browser.url = req.url
        }

        button_home.onClicked: {
            browser.url = "http://172.16.1.111:8888/config/student_detect_trace.config/index"
        }

        button_refresh.onClicked: {
            browser.url = input_url.text;
        }

        ptz_down.onPressedChanged: {
            if (ptz_down.pressed) {
                ptz.down(1);
            }
            else {
                ptz.stop();

                var pos = ptz.get_pos();
                info.text = "down stopped: pos=(" + pos.x + ", " + pos.y + ")"
            }
        }

        ptz_up.onPressedChanged: {
            if (ptz_up.pressed) {
                ptz.up(1);
            }
            else {
                ptz.stop();

                var pos = ptz.get_pos();
                info.text = "up stopped: pos=(" + pos.x + ", " + pos.y + ")"
            }
        }

        ptz_left.onPressedChanged: {
            if (ptz_left.pressed) {
                ptz.left(1);
            }
            else {
                ptz.stop();

                var pos = ptz.get_pos();
                info.text = "left stopped: pos=(" + pos.x + ", " + pos.y + ")"
            }
        }

        ptz_right.onPressedChanged: {
            if (ptz_right.pressed) {
                ptz.right(1);
            }
            else {
                ptz.stop();

                var pos = ptz.get_pos();
                info.text = "right stopped: pos=(" + pos.x + ", " + pos.y + ")"
            }
        }

        ptz_zoom_tele.onPressedChanged: {
            if (ptz_zoom_tele.pressed) {
                ptz.zoom_tele(1);
            }
            else {
                ptz.zoom_stop();

                var zoom = ptz.get_zoom();
                info.text = "zoom tele: zoom=" + zoom.zoom;
            }
        }

        ptz_zoom_wide.onPressedChanged: {
            if (ptz_zoom_wide.pressed) {
                ptz.zoom_wide(1);
            }
            else {
                ptz.zoom_stop();

                var zoom = ptz.get_zoom();
                info.text = "zoom wide: zoom=" + zoom.zoom;
            }
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

        det_factor_00.onValueChanged: {
            var v = det_factor_00.value;
            if (player1.det_enabled) {
                // TODO: notify ..
            }
        }

        det_factor_05.onValueChanged: {
            var v = det_factor_05.value;
            if (player1.det_enabled) {

            }
        }

        det_thres_area.onValueChanged: {
            var v = det_thres_area.value;
            if (player1.det_enabled) {

            }
        }

        det_thres_dis.onValueChanged: {
            var v = det_thres_dis.value;
            if (player1.det_enabled) {

            }
        }
    }

    Component.onCompleted: {
        // 此时加载完成???
        var ptz_url = kvc.get("ptz_serial_name");
        messageDialog.show("try to open ptz url:" + ptz_url);
        ptz.url = ptz_url;
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
