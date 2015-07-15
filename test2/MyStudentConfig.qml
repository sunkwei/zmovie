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
            height: 800;  // 包含数个 MyKVPair + Button 的高度和 ..

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

            /**
            // video_width, video_height
            MyKVPair {
                id: id_kv_video_width;
                title: "探测源视频宽";
                key: "video_width";
                value: kvc.get(key);
                desc: "探测源宽度，保证480，不要修改!!!";
            }

            MyKVPair {
                id: id_kv_video_hegiht;
                title: "探测源视频高";
                key: "video_height";
                value: kvc.get(key);
                desc: "探测源高度，保证270，不要修改!!!";
            }
            */

            // thres_dis
            MyKVPair {
                id: id_kv_thres_dis;
                title: "距离阈值";
                key: "thres_dis";
                value: kvc.get(key)
                desc: "(修改立即生效)经验值在 20 到 40 之间，一般选择25吧";

                onKvalueChanged: {
                    if (player) {
                        var detector = player.item.get_detector_player();
                        if (detector) {
                            var thres_dis = new_value;
                            var thres_area = kvc.get("thres_area");
                            var factor_0 = kvc.get("factor_0");
                            var factor_05 = kvc.get("factor_05");
                            detector.det_set_params(thres_dis, thres_area, factor_0, factor_05);
                        }
                    }
                }
            }

            MyKVPair {
                id: id_kv_thres_area;
                title: "面积阈值"
                key: "thres_area";
                value: kvc.get(key);
                desc: "(修改立即生效)经验值在 3000 到 8000 之间，建议选择 5000";

                onKvalueChanged: {
                    if (player) {
                        var detector = player.item.get_detector_player();
                        if (detector) {
                            var thres_dis = kvc.get("thres_dis");
                            var thres_area = new_value;
                            var factor_0 = kvc.get("factor_0");
                            var factor_05 = kvc.get("factor_05");
                            detector.det_set_params(thres_dis, thres_area, factor_0, factor_05);
                        }
                    }
                }
            }

            MyKVPair {
                id: id_kv_factor_05;
                title: "中间系数"
                key: "factor_05";
                value: kvc.get(key);
                desc: "(修改立即生效)建议选择 0.4";

                onKvalueChanged: {
                    if (player) {
                        var detector = player.item.get_detector_player();
                        if (detector) {
                            var thres_dis = kvc.get("thres_dis");
                            var thres_area = kvc.get("thres_area");
                            var factor_0 = kvc.get("factor_0");
                            var factor_05 = new_value;
                            detector.det_set_params(thres_dis, thres_area, factor_0, factor_05);
                        }
                    }
                }
            }

            MyKVPair {
                id: id_kv_factor_0;
                title: "后排系数"
                key: "factor_0";
                value: kvc.get(key);
                desc: "(修改立即生效)一般选择 0.2";

                onKvalueChanged: {
                    if (player) {
                        var detector = player.item.get_detector_player();
                        if (detector) {
                            var thres_dis = kvc.get("thres_dis");
                            var thres_area = kvc.get("thres_area");
                            var factor_0 = new_value;
                            var factor_05 = kvc.get("factor_05");
                            detector.det_set_params(thres_dis, thres_area, factor_0, factor_05);
                        }
                    }
                }
            }

            MyKVPair {
                id: id_kv_up_angle;
                title: "站立角度";
                key: "up_angle";
                value: kvc.get(key);
                desc: "一般110，如果看到人站立后，较倾斜，应该增加这个值"
            }

            MyKVPair {
                id: id_kv_up_tune_factor;
                title: "站立微调系数";
                key: "up_tune_factor";
                value: kvc.get(key);
                desc: "一般0.7，如果发现坐下很容易检测到，但站立很难检测到，就减小这个值"
            }

            MyKVPair {
                id: id_kv_lr_tune_factor;
                title: "左右晃动系数";
                key: "lr_tune_factor";
                value: kvc.get(key);
                desc: "一般2.0，如果发现站立后左右晃动容易误判为坐下，则增加这个值";
            }

            MyKVPair {
                id: id_kv_up_aspect;
                title: "前后晃动系数";
                key: "up_aspect";
                value: kvc.get(key);
                desc: "一般设置为1.0，如果发现坐下后，前后晃动导致误检为站立，则减小该值";
            }

            MyKVPair {
                id: id_kv_debug_log;
                title: "是否记录日志";
                key: "debug_log";
                value: kvc.get(key);
                desc: "1 启用日志，0 禁用日志，日志将保存在 runtime\bin\detlog\ 目录下";
            }
        }
    }

    Component.onCompleted: {
        if (infos) {
            infos.text = "正在进行学生标定";
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
