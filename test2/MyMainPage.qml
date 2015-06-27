import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtWebKit 3.0
import zonekey.qd 1.4

/** 标定界面，上方为 Player，下方为浏览器 */
ColumnLayout {
    id: main_page;
    visible: false;
    spacing: 2;
    anchors.fill: parent
    property KVConfig kvconfig

    // 搞个特效 ..
    PropertyAnimation {
        target: main_page
        property: "opacity";
        duration: 1000;
        from: 0; to: 1;
        easing.type: Easing.InOutQuad ;
        running: true
    }

    MyPlayers {
        id: players;
        Layout.alignment: Qt.AlignCenter
        Layout.fillWidth: true;
        Layout.preferredHeight: 300
        kvconfig: kvconfig
    }

    ScrollView {
        Layout.alignment: Qt.AlignCenter
        Layout.fillWidth: true;
        Layout.fillHeight: true;

        WebView {
            id: browser;
            anchors.fill: parent;
            url: kvconfig.get("url");
        }
    }
}
