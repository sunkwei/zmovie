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
            title: "学生标定步骤：";
            Layout.fillWidth: true;
            Layout.preferredHeight: 40;

            /** 晕死，互斥的RadioButton Group中，当一个 checked 后，前一个才收到 unchecked
             */

            RowLayout {
                ExclusiveGroup { id: tabPositionGroup }

                function func_unchecked(who)
                {
                    console.log("func_unchecked ..." + who);
                    var pos;

                    // 这里保存 ...
                    if (who === btn_draw_detect_range) {
                        //player_detector.cl_save();
                        var desc = player_detector.cl_points_desc();
                        kvc.set("calibration_data", desc);
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
                    else if (who === btn_verify) {
                        player_detector.det_enabled = false;
                    }
                }

                function func_checked(who)
                {
                    console.log("func_changed ..." + who);

                    // 修改 player 显示模式 ...
                    if (who === btn_draw_detect_range) {
                        if (infos) {
                            infos.text = "使用鼠标，在上方左侧视频中，画出学生探测区";
                        }

                        player_detector.cl_enabled = true;
                    }
                    else if (who === btn_init_ptz) {
                        if (infos) {
                            infos.text = "转动云台，使右侧视频正对着学生区中心位置，并且具有何时的倍率";
                        }

                        player_detector.cl_enabled = false;

                        var x = kvc.get("ptz_init_x");
                        var y = kvc.get("ptz_init_y");
                        var z = kvc.get("ptz_init_z");
                        ptz_controls.reset(x, y, z);
                    }
                    else if (who === btn_verify) {
                        if (infos) {
                            infos.text = "在教室不同位置坐站进行验证，看站立是否出现红框，坐下是否消失"
                        }

                        player_detector.det_enabled = true;
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
                    id: btn_verify;
                    text: "校验学生探测";
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
                            if (this == parent.curr_checked) {
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
        if (btn_draw_detect_range.checked) {
            btn_draw_detect_range.checked = false;
        }

        if (btn_init_ptz.checked) {
            btn_init_ptz.checked = false;
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
