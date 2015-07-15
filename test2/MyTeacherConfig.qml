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
        anchors.fill: parent;

        ColumnLayout {
            id: teachers_config;
            height: 750;  // 包含数个 MyKVPair + Button 的高度和 ..

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
                title: "人体宽度占图像像素个数";
                key: "t_body_width";
                value: kvc.get(key)
                desc: "一般选择 40";
            }

            MyKVPair {
                id: id_kv_y_adjust;
                title: "是否支持教师身高微调";
                key: "y_adjust";
                value: kvc.get(key);
                desc: "0 不启用，1 启用，默认 0";
            }

            MyKVPair {
                id: id_kv_y_adjust_threshold;
                title: "启用身高微调阈值"
                key: "y_adjust_threshold";
                value: kvc.get(key);
                desc: "当老师高度变化大于该值时才进行调整";
            }

            MyKVPair {
                id: id_kv_y_adjust_factor10_;
                title: "教师高度调节的幅度";
                key: "y_adjust_factor10_";
                value: kvc.get(key);
                desc: "老师高度调整时，调整的幅度, 一般1到30就可以了，1基本不动，30动的幅度比较大";
            }
        }
    }
    Component.onCompleted: {
        if (infos) {
            infos.text = "正在进行教师标定";
        }
    }

    Component.onDestruction: {

    }

    function save()
    {
        if (kvc.save() < 0) {
            infos.text = "保存配置参数失败!!!!";
        }
        else {
            infos.text = "保存配置成功";
        }
    }

    function cancel()
    {
        kvc.reload();
        infos.text = "已经取消";
    }
}
