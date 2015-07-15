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

            //bg_learning_rate=0.001 背景更新速度
            MyKVPair {
                id: id_kv_bg_learning_rate;
                title: "背景更新速度";
                key: "bg_learning_rate";
                value: kvc.get(key);
                desc: "除非你知道正在干啥，否则不要修改该值";
            }

            // bg_mog_threshold=135 图像二值化阈值
            MyKVPair {
                id: id_kv_bg_mog_threshold
                title: "图像二值化阈值";
                key: "bg_mog_threshold";
                value: kvc.get(key);
                desc: "除非你知道正在干啥，否则不要修改该值";
            }

            //bg_varThreshold=60 高斯动态背景阈值
            MyKVPair {
                id: id_kv_bg_varThreshold;
                title: "高斯动态背景阈值";
                key: "bg_varThreshold";
                value: kvc.get(key);
                desc: "除非你知道正在干啥，否则不要修改该值";
            }
        }
    }
    Component.onCompleted: {
        if (infos) {
            infos.text = "正在进行板书标定";
        }
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
