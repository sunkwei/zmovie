import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Rectangle {
    width: 600
    height: 400


    ScrollView {
        anchors.fill: parent;

        ColumnLayout {
            height: 900
            width: 600;
            id: m0

            Button {
                width: 100;
                height: 300;
                Layout.fillWidth: true;
                text: "button 1";
                id: b1;
            }

            Button {
                id: b2;
                height: 300;
                Layout.fillWidth: true;
                text: "button 2";
            }

            Button {
                id: b3;
                height: 300;
                Layout.fillWidth: true;
                text: "button 3";
            }

            Component.onCompleted: {
                console.log("m0:" + m0.width + "," + m0.height);
            }
        }
    }
}
