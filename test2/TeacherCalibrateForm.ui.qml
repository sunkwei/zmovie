import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtWebKit 3.0

Item {
    id: teacher_calibration_form;
    anchors.fill: parent

    /** 教师标定界面, 主要分两层，上面是两个 Player，分别显示探测和跟踪源.
        下层显示是一个浏览器，显示所有配置项.

     */
    ColumnLayout {

        RowLayout {
            id: players
            height: 270  // 高度就是视频的高度
        }

        WebView {
            id: browser
        }
    }
}
