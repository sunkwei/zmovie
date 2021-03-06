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

                MouseArea {
                    anchors.fill: parent;
                    acceptedButtons: Qt.LeftButton | Qt.RightButton;
                    onReleased: {
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
            title: "教师标定步骤：";
            Layout.fillWidth: true;
            Layout.preferredHeight: 40;

            /** 晕死，互斥的RadioButton Group中，当一个 checked 后，前一个才收到 unchecked
             */

            RowLayout {
                ExclusiveGroup { id: tabPositionGroup }

                function func_unchecked(who)
                {
                    console.log("func_unchecked ..." + who)
                    var pos;

                    // 这里保存 ...
                    if (who === btn_draw_detect_range) {
                        //player_detector.cl_save();
                        var desc = player_detector.cl_points_desc;
                        kvc.set("calibration_data", desc);
                        player_detector.cl_enabled = false;
                    }
                    else if (who === btn_init_ptz) {
                        pos = ptz_controls.ptz.get_pos();
                        if (pos.err != 0) {
                            infos.text = "无法得到云台的当前位置";
                            return;
                        }

                        kvc.set("ptz_init_x", pos.x);
                        kvc.set("ptz_init_y", pos.y);

                        var zoom = ptz_controls.ptz.get_zoom();
                        if (zoom.err != 0) {
                            infos.text = "无法得到云台当前的倍率";
                            return;
                        }

                        kvc.set("ptz_init_z", zoom.zoom);
                    }
                    else if (who === btn_ptz_left) {
                        pos = ptz_controls.ptz.get_pos();
                        if (pos.err !== 0) {
                            infos.text = "无法得到云台当前位置";
                            return;
                        }

                        kvc.set("ptz_init_left", pos.x);
                    }
                    else if (who === btn_ptz_right) {
                        pos = ptz_controls.ptz.get_pos();
                        if (pos.err !== 0) {
                            infos.text = "无法得到云台当前位置";
                            return;
                        }

                        kvc.set("ptz_init_right", pos.x);
                    }
                    else if (who === btn_y_adjust) {
                        var desc = player_detector.cl_points_desc;
                        kvc.set("upbody_calibration_data", desc);
                        player_detector.cl_enabled = false;
                    }
                }

                function func_checked(who)
                {
                    console.log("func_changed ..." + who);

                    // 修改 player 显示模式 ...
                    if (who === btn_draw_detect_range) {
                        if (infos) {
                            infos.text = "使用鼠标，在上方左侧视频中，画出教师探测区";
                        }

                        player_detector.cl_enabled = true;
                        player_detector.cl_points_desc = kvc.get("calibration_data");
                    }
                    else if (who === btn_init_ptz) {
                        if (infos) {
                            infos.text = "转动云台，使右侧视频正对着黑板，并且调整到合适倍率";
                        }

                        player_detector.cl_enabled = false;

                        var x = kvc.get("ptz_init_x");
                        var y = kvc.get("ptz_init_y");
                        var z = kvc.get("ptz_init_z");
                        ptz_controls.reset(x, y, z);
                    }
                    else if (who === btn_ptz_left) {
                        if (infos) {
                            infos.text = "转动云台，使右侧视频中红线对准教师探测区的最左侧的黄线";
                        }

                        // 在探测源中间划红色竖线，方便左右转动时对齐 ...
                        player_tracing.dr_push({"name":"line", "x0":240, "y0":0, "x1":240, "y1":270});
                        player_detector.cl_enabled = true;

                        var x = kvc.get("ptz_init_left");
                        var y = kvc.get("ptz_init_y");
                        ptz.set_pos(x, y, 36, 36);
                    }
                    else if (who === btn_ptz_right) {
                        if (infos) {
                            infos.text = "转动云台，使右侧视频中红线对准教师探测区的最右侧的黄线";
                        }

                        // 在探测源中间划红色竖线，方便左右转动时对齐 ...
                        player_tracing.dr_push({"name":"line", "x0":240, "y0":0, "x1":240, "y1":270})
                        player_detector.cl_enabled = true;

                        var x = kvc.get("ptz_init_right");
                        var y = kvc.get("ptz_init_y");
                        ptz.set_pos(x, y, 36, 36);
                    }
                    else if (who === btn_y_adjust) {
                        if (infos) {
                            infos.text = "画出矩形，标定教师高度调节的范围和高度";
                        }

                        // upbody_calibration_data=100,44;377,45;275,98,107,98;
                        var upbody_calibration_data = kvc.get("upbody_calibration_data");
                        player_detector.cl_enabled = true;
                        player_detector.cl_points_desc = upbody_calibration_data;
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
                    id: btn_draw_detect_range;
                    text: "设置探测区";
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
                    id: btn_init_ptz;
                    text: "初始化云台";
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
                    id: btn_ptz_left;
                    text: "左侧标定";
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
                    id: btn_ptz_right;
                    text: "右侧标定";
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
                    id: btn_y_adjust;
                    text: "教师高度标定";
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
            }
        }
    }

    function save()
    {
        // 这里需要确认保存所有 ...
        if (btn_draw_detect_range.checked) {
            btn_draw_detect_range.checked = false;
        }

        if (btn_init_ptz.checked) {
            btn_init_ptz.checked = false;
        }

        if (btn_ptz_left.checked) {
            btn_ptz_left.checked = false;
        }

        if (btn_ptz_right.checked) {
            btn_ptz_right.checked = false;
        }

        if (btn_y_adjust.checked) {
            btn_y_adjust.checked = false;
        }
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
