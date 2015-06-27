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

    KVConfig {
        id: kvc;
        fname: "test.config"
    }

    MyLogin {
        id: login;
        visible: false;
        anchors.centerIn: parent;
        kvconfig: kvc
        onMyOk: {
            login.visible = false;
            main_page.visible = true;
        }
    }

    MyMainPage {
        id: main_page;
        anchors.fill: parent;
        kvconfig: kvc
    }

    // 此时加载完成???
    Component.onCompleted: {
        main_page.visible = false;
        login.visible = true;
    }
}
