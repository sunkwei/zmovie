import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import zonekey.qd 1.4

/** 标定界面，上方为 Player，下方为配置项 */
ColumnLayout {
    id: main_page;
    spacing: 2;
    anchors.fill: parent
    width: 1280;

    property KVConfig kvc  // 全局配置 ..
    property string who;    // 对应着被标定者，目前为 teacher, student, bd

    // 视频区 ..
    Loader {
        id: loader_players;
        Layout.alignment: Qt.AlignCenter
        Layout.fillWidth: true;
        Layout.preferredHeight: 320;
        width: 300;
    }

    // 配置项编辑 ..
    Loader {
        id: loader_config;
        Layout.fillWidth: true;
        Layout.fillHeight: true;
        anchors.bottom: parent.bottom;
        width: 1280;

        Component.onCompleted: {
            if (who == "teacher") {
                loader_config.setSource("MyTeacherConfig.qml", { "kvc": kvc, "player": loader_players});
            }
            else if (who == "student") {
                loader_config.setSource("MyStudentConfig.qml", { "kvc": kvc, "player": loader_players });
            }
            else if (who == "bd") {
                loader_config.setSource("MyBDConfig.qml", { "kvc": kvc, "player": loader_players });
            }
            else if (who == "flipped") {
                loader_config.setSource("MyStudentFlipConfig.qml", { "kvc": kvc, "player": loader_players} );
            }
        }
    }

    function save()
    {
        // 保存所有 ...
        loader_players.item.save();
        loader_config.item.save();
    }

    function cancel()
    {
        // 放弃修改 ...
        loader_players.item.cancel();
        loader_config.item.cancel();
    }

    Component.onCompleted: {
        if (who == "teacher") {
            loader_players.setSource("MyPlayersTeacher.qml");
        }
        else if (who == "student") {
            loader_players.setSource("MyPlayersStudent.qml");
        }
        else if (who == "bd") {
            loader_players.setSource("MyPlayersBD.qml");
        }
        else if (who == "flipped") {
            loader_players.setSource("MyPlayersStudentFlip.qml");
        }
    }
}
