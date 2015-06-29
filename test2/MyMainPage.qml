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
    property string who;    // 对应着被标定者，目前为 teacher, student, bd
    property string remote_config_url;  // 目标 url

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
            url: remote_config_url;
        }
    }
}
