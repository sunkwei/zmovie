import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import zonekey.qd 1.4
import QtQuick.Controls.Styles 1.2

/** 教师标定属性 ... */

BorderImage {
    id: sv;
    anchors.fill: parent;
    source: "images/toolbar.png"
    border.bottom: 8;

    property KVConfig kvc;          // 全局配置项 ...
    property var player;       // 对应着播放器 ...

    ScrollView {
        id: sv0;
        anchors.fill: parent

        ColumnLayout {
            id: teachers_config;
            height: 500;  // 包含数个 MyKVPair + Button 的高度和 ..

            // 云台服务 ..
            MyKVPair {
                id: id_kv_ptz_serial_name;
                title: "云台服务地址";
                key: "ptz_serial_name";
                value: kvc.get(key);
                desc: "输入云台服务的url，一般格式为 http://<ip>:10003/ptz/teacher";
            }

            // 探测源 url
            MyKVPair {
                id: id_kv_video_source;
                title: "探测源url";
                key: "video_source";
                value: kvc.get(key);
                desc: "输入教师探测ipcam的url，一般格式为 rtsp://<ip>/av0_0";
            }

            // 跟踪源 url
            MyKVPair {
                id: id_kv_video_source_tracing;
                title: "跟踪源url";
                key: "video_source_tracing";
                value: kvc.get(key);
                desc: "输入录播机教师特写的直播流地址，支持rtsp/rtmp两种格式";
            }

            // trace_speeds
            MyKVPair {
                id: id_kv_trace_speeds;
                title: "跟踪云台转动速度";
                key: "trace_speeds";
                value: kvc.get(key);
                desc: "云台跟踪速度....";
            }

            // luv_u_max
            MyKVPair {
                id: id_kv_luv_L;
                title: "亮度信息"
                key: "luv_L";
                value: kvc.get(key);
                desc: "一般选择 50";
            }

            MyKVPair {
                id: id_kv_luv_u_max;
                title: "色度信息"
                key: "luv_u_max";
                value: kvc.get(key);
                desc: "一般选择 23";
            }

            MyKVPair {
                id: id_kv_luv_v_max;
                title: "色度信息"
                key: "luv_v_max";
                value: kvc.get(key);
                desc: "一般选择 23";
            }

            MyKVPair {
                id: id_kv_t_body_width;
                title: "人体宽度占图像像素个数"
                key: "t_body_width";
                value: kvc.get(key);
                desc: "一般选择 40";
            }


            // 保存配置，并且返回 ..
            Button {
                text: "保存设置并返回";
                style: touchStyle;

                onClicked: {
                    focus = true;   // 这样能强制使 ...

                    if (player) {
                        player.item.save();
                    }

                    if (kvc.save() < 0) {
                        infos.text = "保存配置参数失败!!!!";
                    }
                    else {
                        infos.text = "保存配置成功";
                    }

                    if (stackView) {
                        stackView.pop();
                    }
                }
            }

            // 放弃保存
            Button {
                text: "放弃设置并返回";
                style: touchStyle;

                onClicked: {
                    if (stackView) {
                        stackView.pop();
                    }
                    if (kvc.reload() < 0) {
                        infos.text = "重新加载配置失败!!!!";
                    }
                    else {
                        infos.text = "已经取消保存";
                    }
                }
            }

            Component {
                id: touchStyle
                ButtonStyle {
                    panel: Item {
                        implicitHeight: 30
                        implicitWidth: 260
                        BorderImage {
                            anchors.fill: parent
                            antialiasing: true
                            border.bottom: 8
                            border.top: 8
                            border.left: 8
                            border.right: 8
                            anchors.margins: control.pressed ? -4 : 0
                            source: control.pressed ? "../images/button_pressed.png" : "../images/button_default.png"
                            Text {
                                text: control.text
                                anchors.centerIn: parent
                                color: "white"
                                font.pixelSize: 19
                                renderType: Text.NativeRendering
                            }
                        }
                    }
                }
            }
        }
    }
    Component.onCompleted: {
        if (infos) {
            infos.text = "正在进行教师标定";
        }
    }
}
