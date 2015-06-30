import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import zonekey.qd 1.4

/** 保存两个 player，左侧为探测源，右侧为跟踪源视频 ... */
Item {
    height: 300; // 强制高度

    RowLayout {
        spacing: 30
        anchors.fill: parent;

        ColumnLayout {
            Player {
                id: player_detector;
                width: 480;
                height: 270;
                det_enabled: false;
                cl_enabled: false;
                fillColor: "#ffffff"
            }
        }

        ColumnLayout {
            Player {
                id: player_tracing;
                width: 480;
                height: 270;
                det_enabled: false;
                cl_enabled: false;
                fillColor: "#ffffff"
            }
        }

        MyPtzControls {
            id: ptz_controls;
            anchors.right: parent.right
        }
    }

    Component.onCompleted: {
        if (!kvc) {
            console.error("kvc is NULL????");
        }

        player_detector.url = kvc.get("video_source");
        console.log("video_source url:" + player_detector.url);
        player_detector.play();

        player_tracing.url = kvc.get("video_source_tracing");
        console.log("tracing url:" + player_tracing.url);
        player_tracing.play();
    }
}
