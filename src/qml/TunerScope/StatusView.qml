import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: statusViewRoot
    anchors.fill: parent

    implicitHeight: statusItemLayout.implicitHeight + 4
    implicitWidth: statusItemLayout.implicitWidth

    RowLayout {
        id: statusItemLayout
        anchors.fill: parent
        anchors.leftMargin: 5
        anchors.rightMargin: 2
        anchors.topMargin: 2
        anchors.bottomMargin: 2
        spacing: 10


        Text {
            text: settingsModel.deviceName
            color: "lightsteelblue"
            Layout.fillWidth: true
            font: fixedFont
            renderType: Text.NativeRendering
        }
        Text {
            text: {"Sample Rate = " + settingsModel.sampleRate}
            color: "lightsteelblue"
            Layout.alignment: Qt.AlignRight
            font: fixedFont
            renderType: Text.NativeRendering
        }
        Text {
            text: "FFT size = " + settingsModel.fftSize
            color: "lightsteelblue"
            Layout.alignment: Qt.AlignRight
            font: fixedFont
            renderType: Text.NativeRendering
        }
        Button  {
            id: settingsButton
            text: qsTr("Settings")
            Layout.alignment: Qt.AlignRight
            Layout.fillWidth: false

            // For Windows style this looks better

            background: Rectangle {
                radius: 0
                color: "gray"
                //border.color: "darkgray"
                border.color: settingsButton.hovered ? "skyblue" : "darkgray"
                border.width: 1
            }


            implicitHeight: fixedFontHeight * 1.5

            onClicked: {
                settings.visible = true;
                //settings.z = 1
                mainLayout.visible = false;
            }
        }
    }
}
