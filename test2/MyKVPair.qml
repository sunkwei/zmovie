import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

Rectangle {
    property string key;
    property string value;

    height: 30

    RowLayout {
        anchors.fill: parent;
        spacing: 3;

        Text {
            id: id_key;
            text: key;
            Layout.minimumWidth: 120
        }

        TextField {
            id: id_value;
            text: value;
        }
    }
}
