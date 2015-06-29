import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

/** 教师标定属性 ... */

ScrollView {
    property var json_of_config;    // json 格式配置项 ...

    Component.onCompleted: {

    }

    ColumnLayout {
        id: all_config;
        spacing: 2;
        anchors.fill: parent;

        MyKVPair {
            key: "ptz_serial_name";
            value: json_of_config.ptz_serial_name;
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true;
        }

        MyKVPair {
            key: "ptz_addr";
            value: json_of_config.ptz_addr;
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true;
        }
    }
}
