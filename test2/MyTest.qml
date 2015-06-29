import QtQuick 2.0

Rectangle {
    width: 600
    height: 400

    property string json_str_config

    MyTeacherConfig {
        json_of_config: JSON.parse(json_str_config);
        anchors.fill: parent;
    }

    Component.onCompleted: {
        json_str_config = "{\"ptz_serial_name\": \"http://...\", \"ptz_addr\": 1 }";
    }
}
