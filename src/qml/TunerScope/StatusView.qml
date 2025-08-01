import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    anchors.fill: parent

    RowLayout {
        anchors.fill: parent

        Text {
            text: "Device name"
            color: "lightsteelblue"
            Layout.fillWidth: true
        }
        Text {
            text: "Sample rate"
            color: "lightsteelblue"
            Layout.alignment: Qt.AlignRight
        }
        Text {
            text: "fft size"
            color: "lightsteelblue"
            Layout.alignment: Qt.AlignRight
        }
        Button  {
            text: "Settings"
            Layout.alignment: Qt.AlignRight
            Layout.fillWidth: false

            /*
            background: Rectangle {
                radius: 5
                color: "darkgray"
            }
            */

            onClicked: {
                settings.visible = true;
                //settings.z = 1
                mainLayout.visible = false;
            }
        }
    }
}
