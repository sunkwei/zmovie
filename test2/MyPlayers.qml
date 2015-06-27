import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import zonekey.qd 1.4

/** 保存两个 player，左侧为探测源，右侧为跟踪源视频 ... */
Item {
    height: 300; // 强制高度

    property KVConfig kvconfig;

    RowLayout {
        spacing: 30
        anchors.fill: parents;

        ColumnLayout {
            Text {
                text: "detecting source ...";
                height: 25
            }

            Player {
                id: player_detector;
                width: 480;
                height: 270;
                det_enabled: false;
                cl_enabled: false;
                fillColor: "#ff0000"
            }
        }

        ColumnLayout {
            Text {
                text: "tracing source ...";
                height: 25;
            }

            Player {
                id: player_tracing;
                width: 480;
                height: 270;
                det_enabled: false;
                cl_enabled: false;
                fillColor: "#ffff00"
            }
        }

        MyPtzControls {
            id: ptz_controls;
            anchors.right: parent.right
        }
    }
}
