import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import zonekey.qd 1.4

/** 标定界面，上方为 Player，下方为浏览器 */
ColumnLayout {
    id: main_page;
    spacing: 2;
    anchors.fill: parent

    property KVConfig kvc  // 全局配置 ..
    property string who;    // 对应着被标定者，目前为 teacher, student, bd

    MyPlayers {
        id: players;
        Layout.alignment: Qt.AlignCenter
        Layout.fillWidth: true;
        Layout.preferredHeight: 300
    }

    Item {
        Layout.fillWidth: true;
        Layout.fillHeight: true;

        Loader {
            id: loader;
            anchors.fill: parent;

            Component.onCompleted: {
                if (who == "teacher") {
                    loader.setSource("MyTeacherConfig.qml", { "kvc": kvc });
                }
                else if (who == "student") {
                    loader.setSource("MyStudentConfig.qml", { "kvc": kvc });
                }
            }
        }
    }
}
