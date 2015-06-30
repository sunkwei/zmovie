import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import zonekey.qd 1.4

/** 保存两个 player，左侧为探测源，右侧为跟踪源视频 ... */
Item {
    height: 350; // 强制高度

    Column {
        RowLayout {
            //anchors.fill: parent;
            Layout.fillWidth: true;
            Layout.preferredHeight: 300;

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

        GroupBox {
            title: "steps";
            Layout.fillWidth: true;
            Layout.fillHeight: true;
            Layout.preferredHeight: 50;

            RowLayout {
                ExclusiveGroup {
                    id: tabPositionGroup
                }
                RadioButton {
                    text: "step1";
                    checked: true;
                    exclusiveGroup: tabPositionGroup
                }

                RadioButton {
                    text: "step2";
                    exclusiveGroup: tabPositionGroup
                }
            }
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
