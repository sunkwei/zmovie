import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import zonekey.qd 1.4

Item {
    property string key;
    property string title;
    property string value;
    property string desc;

    signal kvalueChanged(string new_value);

    width: 1024;
    height: 50;

    RowLayout {
        anchors.fill: parent;
        spacing: 2;

        Text {
            id: id_key;
            text: title;
            Layout.preferredWidth: 180;
            Layout.fillWidth: true;
            font.pointSize: 13;
            color: "white";
        }

        TextField {
            id: id_value;
            text: value;
            anchors.left: id_key.right;
            Layout.preferredWidth: 500;
            Layout.fillWidth: true;
            style: touchStyle_txt;
            font.pointSize: 13;

            onTextChanged: {
                kvc.set(key, text); // 总是立即写入 kvc ...
                kvalueChanged(text);    // 随时生效 ...
            }
        }

        Text {
            id: id_desc;
            text: desc;
            Layout.fillWidth: true;
            font.pointSize: 13
            color: "white"
        }

        Component {
            id: touchStyle_txt

            TextFieldStyle {
                textColor: "white";
                background: Item {
                    implicitHeight: 50
                    implicitWidth: 320
                    BorderImage {
                        source: "../images/textinput.png"
                        border.left: 8
                        border.right: 8
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                    }
                }
            }
        }
    }
}
