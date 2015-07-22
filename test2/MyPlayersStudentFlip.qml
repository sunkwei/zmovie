import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import zonekey.qd 1.4

/** 保存两个 player，左侧为探测源，右侧为跟踪源视频 ... */
Item {
    id: players;
    height: 350; // 强制高度..

    ColumnLayout {
        RowLayout {
            height: 270;
            width: parent.width;

            // 探测源 ..
            Player {
                id: player_detector;
                width: 480;
                height: 270;
                det_enabled: false;
                cl_enabled: false;
                fillColor: "#ffffff"

                property point rt_pressed_point: Qt.point(-1, -1);

                MouseArea {
                    anchors.fill: parent;
                    acceptedButtons: Qt.LeftButton | Qt.RightButton;
                    onReleased: {
                        // 标定点 ...
                        if (parent.cl_enabled) {
                            if (mouse.button === Qt.LeftButton) {
                                // add
                                parent.cl_push_point(mouse.x, mouse.y);
                            }
                            else if (mouse.button === Qt.RightButton) {
                                // remove
                                parent.cl_pop_point();
                            }
                        }

                        // 画橡皮筋 ...
                        if (parent.rt_enabled) {
                            hoverEnabled = false;
                            parent.rt_tracked(parent.rt_pressed_point, Qt.point(mouse.x, mouse.y));
                        }
                    }

                    onPressed: {
                        if (parent.rt_enabled) {
                            hoverEnabled = true;
                            parent.rt_pressed_point = Qt.point(mouse.x, mouse.y);
                        }
                    }

                    onPositionChanged: {
                        if (parent.rt_enabled) {
                            parent.rt_tracking(parent.rt_pressed_point, Qt.point(mouse.x, mouse.y));
                        }
                    }
                }
            }

            // 跟踪源 ..
            Player {
                id: player_tracing;
                width: 480;
                height: 270;
                det_enabled: false;
                cl_enabled: false;
                fillColor: "#ffffff"
            }

            Ptz {
                id: ptz;
                url: kvc.get("ptz_serial_name");
            }

            // 控制云台组件 ..
            MyPtzControls {
                id: ptz_controls;
                ptz: ptz;
                Layout.preferredWidth: 500;
                Layout.preferredHeight: 270;
            }
        }

        GroupBox {
            title: "学生标定步骤：";
            Layout.fillWidth: true;
            Layout.preferredHeight: 40;

            /** 晕死，互斥的RadioButton Group中，当一个 checked 后，前一个才收到 unchecked
             */

            RowLayout {
                ExclusiveGroup { id: tabPositionGroup }

                function func_unchecked(who)
                {
                    var pos;

                    // 这里保存 ...
                    if (who === btn_flipped_ploy_1) {
                        var desc = player_detector.cl_points_desc;
                        kvc.set("flipped_ploy_1", desc);
                        player_detector.cl_enabled = false;
                        //ptz_controls.ptz.preset_save(1);
                    }
                    if (who === btn_flipped_ploy_2) {
                        var desc = player_detector.cl_points_desc;
                        kvc.set("flipped_ploy_2", desc);
                        player_detector.cl_enabled = false;
                        //ptz_controls.ptz.preset_save(2);
                    }
                    if (who === btn_flipped_ploy_3) {
                        var desc = player_detector.cl_points_desc;
                        kvc.set("flipped_ploy_3", desc);
                        player_detector.cl_enabled = false;

                        ptz_controls.ptz.preset_save(3);
                    }
                    if (who === btn_flipped_ploy_4) {
                        var desc = player_detector.cl_points_desc;
                        kvc.set("flipped_ploy_4", desc);
                        player_detector.cl_enabled = false;

                        ptz_controls.ptz.preset_save(4);
                    }
                    if (who === btn_flipped_ploy_5) {
                        var desc = player_detector.cl_points_desc;
                        kvc.set("flipped_ploy_5", desc);
                        player_detector.cl_enabled = false;

                        ptz_controls.ptz.preset_save(5);
                    }
                    if (who === btn_flipped_ploy_6) {
                        var desc = player_detector.cl_points_desc;
                        kvc.set("flipped_ploy_6", desc);
                        player_detector.cl_enabled = false;

                        ptz_controls.ptz.preset_save(6);
                    }
                }

                function func_checked(who)
                {
                    // 修改 player 显示模式 ...
                    if (who === btn_flipped_ploy_1) {
                        infos.text = "使用鼠标，在上方左侧视频中，画出学生组的位置";
                        player_detector.cl_enabled = true;
                        player_detector.cl_points_desc = kvc.get("flipped_ploy_1");
                        //ptz_controls.ptz.preset_call(1);
                    }
                    if (who === btn_flipped_ploy_2) {
                        infos.text = "使用鼠标，在上方左侧视频中，画出学生组的位置";
                        player_detector.cl_enabled = true;
                        player_detector.cl_points_desc = kvc.get("flipped_ploy_2");
                        //ptz_controls.ptz.preset_call(2);
                    }
                    if (who === btn_flipped_ploy_3) {
                        infos.text = "使用鼠标，在上方左侧视频中，画出学生组的位置";
                        player_detector.cl_enabled = true;
                        player_detector.cl_points_desc = kvc.get("flipped_ploy_3");
                        ptz_controls.ptz.preset_call(3);
                    }
                    if (who === btn_flipped_ploy_4) {
                        infos.text = "使用鼠标，在上方左侧视频中，画出学生组的位置";
                        player_detector.cl_enabled = true;
                        player_detector.cl_points_desc = kvc.get("flipped_ploy_4");
                        ptz_controls.ptz.preset_call(4);
                    }
                    if (who === btn_flipped_ploy_5) {
                        infos.text = "使用鼠标，在上方左侧视频中，画出学生组的位置";
                        player_detector.cl_enabled = true;
                        player_detector.cl_points_desc = kvc.get("flipped_ploy_5");
                        ptz_controls.ptz.preset_call(5);
                    }
                    if (who === btn_flipped_ploy_6) {
                        infos.text = "使用鼠标，在上方左侧视频中，画出学生组的位置";
                        player_detector.cl_enabled = true;
                        player_detector.cl_points_desc = kvc.get("flipped_ploy_6");
                        ptz_controls.ptz.preset_call(6);
                    }
                }

                property var curr_checked: null;    // 当前 ..

                Component {
                    id: style_button;
                    RadioButtonStyle {
                        label:Text {
                            color: "white";
                        }
                    }
                }

                RadioButton {
                    id: btn_flipped_ploy_1;
                    text: "组1";
                    exclusiveGroup: tabPositionGroup
                    onCheckedChanged: {
                        if (checked) {
                            if (parent.curr_checked) {
                                parent.func_unchecked(parent.curr_checked);
                            }

                            parent.curr_checked = this;
                            parent.func_checked(this);
                        }
                        else {
                            if (this === parent.curr_checked) {
                                parent.func_unchecked(btn_draw_detect_range);
                            }
                        }
                    }
                }

                RadioButton {
                    id: btn_flipped_ploy_2;
                    text: "组2";
                    exclusiveGroup: tabPositionGroup
                    onCheckedChanged: {
                        if (checked) {
                            if (parent.curr_checked) {
                                parent.func_unchecked(parent.curr_checked);
                            }

                            parent.curr_checked = this;
                            parent.func_checked(this);
                        }
                        else {
                            if (this === parent.curr_checked) {
                                parent.func_unchecked(this);
                            }
                        }
                    }
                }

                RadioButton {
                    id: btn_flipped_ploy_3;
                    text: "组3";
                    exclusiveGroup: tabPositionGroup;
                    onCheckedChanged: {
                        if (checked) {
                            if (parent.curr_checked) {
                                parent.func_unchecked(parent.curr_checked);
                            }

                            parent.curr_checked = this;
                            parent.func_checked(this);
                        }
                        else {
                            if (this === parent.curr_checked) {
                                parent.func_unchecked(this);
                            }
                        }
                    }
                }

                RadioButton {
                    id: btn_flipped_ploy_4;
                    text: "组4";
                    exclusiveGroup: tabPositionGroup;
                    onCheckedChanged: {
                        if (checked) {
                            if (parent.curr_checked) {
                                parent.func_unchecked(parent.curr_checked);
                            }

                            parent.curr_checked = this;
                            parent.func_checked(this);
                        }
                        else {
                            if (this === parent.curr_checked) {
                                parent.func_unchecked(this);
                            }
                        }
                    }
                }

                RadioButton {
                    id: btn_flipped_ploy_5;
                    text: "组5";
                    exclusiveGroup: tabPositionGroup;
                    onCheckedChanged: {
                        if (checked) {
                            if (parent.curr_checked) {
                                parent.func_unchecked(parent.curr_checked);
                            }

                            parent.curr_checked = this;
                            parent.func_checked(this);
                        }
                        else {
                            if (this === parent.curr_checked) {
                                parent.func_unchecked(this);
                            }
                        }
                    }
                }

                RadioButton {
                    id: btn_flipped_ploy_6;
                    text: "组6";
                    exclusiveGroup: tabPositionGroup;
                    onCheckedChanged: {
                        if (checked) {
                            if (parent.curr_checked) {
                                parent.func_unchecked(parent.curr_checked);
                            }

                            parent.curr_checked = this;
                            parent.func_checked(this);
                        }
                        else {
                            if (this === parent.curr_checked) {
                                parent.func_unchecked(this);
                            }
                        }
                    }
                }
            }
        }
    }

    // 仅仅为了给学生修改配置参数，能立即生效而使用，不好 ...
    function get_detector_player()
    {
        return player_detector;
    }

    function save()
    {
        // 这里需要确认保存所有 ...
        if (btn_flipped_ploy_1.checked) btn_flipped_ploy_1.checked = false;
        if (btn_flipped_ploy_2.checked) btn_flipped_ploy_2.checked = false;
        if (btn_flipped_ploy_3.checked) btn_flipped_ploy_3.checked = false;
        if (btn_flipped_ploy_4.checked) btn_flipped_ploy_4.checked = false;
        if (btn_flipped_ploy_5.checked) btn_flipped_ploy_5.checked = false;
        if (btn_flipped_ploy_6.checked) btn_flipped_ploy_6.checked = false;
    }

    function cancel()
    {

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
