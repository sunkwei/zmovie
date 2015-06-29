import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtWebKit 3.0
import zonekey.qd 1.4

ApplicationWindow {
    title: qsTr("zonekey player testing")
    width: 1280
    height: 720
    visible: true

    property string remote_url;

    KVConfig {
        id: kvc;
    }

    MyLogin {
        id: login;
        visible: false;
        anchors.centerIn: parent;
        onMyOk: {
            login.visible = false;
            main_page.visible = true;

            var target_ip = login.ip;
            var who = login.who;
            var fname = who + "_detect_trace.config";

            kvc.fname = fname;
            remote_url = "http://" + target_ip + ":8888/config/" + fname + "/index";
        }
    }

    MyMainPage {
        id: main_page;
        anchors.fill: parent;
        kvconfig: kvc
        remote_config_url: remote_url
    }

    // 此时加载完成???
    Component.onCompleted: {
        main_page.visible = false;
        login.visible = true;
    }
}
