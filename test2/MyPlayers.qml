import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import zonekey.qd 1.4

/** 保存两个 player，左侧为探测源，右侧为跟踪源视频 ... */
Item {
    anchors.fill: parent;

    Text {
        text: "playing ...."
    }

    ColumnLayout {
        spacing: 30

        Row {
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

        Row {
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
    }
}
