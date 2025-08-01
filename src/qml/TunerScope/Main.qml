import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls

Window {
    id: mainWindow
    width: 640
    height: 360
    visible: true
    title: qsTr("TunerScope")
    color: "black"

    Settings {
        id: settings
        visible: false
    }

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        spacing: 0

        Rectangle {
            color: "black"
            Layout.fillHeight: true
            Layout.fillWidth: true
            implicitHeight: 2 * parent.height / 3

            SpectrumView {
                anchors.fill: parent
            }

        }

        Rectangle {
            color: "black"
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            implicitHeight: parent.height / 3

            TunerView {
                anchors.fill: parent
            }
        }

        Rectangle {
            color: "black"
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            implicitHeight: 22

            StatusView {
                anchors.fill: parent
            }
        }
    }
}
