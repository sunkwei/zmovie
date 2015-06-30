import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

Item {
    property string desc;
    property string key;
    property string value;

    signal kvalueChanged;

    RowLayout {
        anchors.fill: parent;
        spacing: 2;

        Text {
            id: id_key;
            text: key;
            Layout.preferredWidth: 120;
        }

        TextField {
            id: id_value;
            text: value;
            anchors.left: id_key.right;
            Layout.preferredWidth: 400;

            onFocusChanged: {
                if (!focus) {
                    if (value !== text) {
                        value = text.trim();
                        kvalueChanged;
                    }
                }
            }
        }

        Text {
            id: id_desc;
            text: desc;
            Layout.fillWidth: true;
        }
    }
}
