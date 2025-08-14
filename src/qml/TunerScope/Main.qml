import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Shapes

// For quick ui style testing
//import QtQuick.Controls.Basic
//import QtQuick.Controls.Fusion
////import QtQuick.Controls.Imagine
////import QtQuick.Controls.Material
import QtQuick.Controls.Universal
//import QtQuick.Controls.Windows

Window {
    id: mainWindow
    width: settingsModel.wndWidth
    height: settingsModel.wndHeight
    visible: true
    title: qsTr("TunerScope")
    color: "black"

    onWidthChanged: {
        settingsModel.wndWidth = width
    }
    onHeightChanged: {
        settingsModel.wndHeight = height
    }

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
            implicitHeight: 3 * parent.height / 4

            SpectrumView {
                anchors.fill: parent
            }
        }

        Rectangle {
            color: "black"
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            implicitHeight: parent.height / 4

            TunerView {
                id: tunerView
                anchors.fill: parent
            }
            Layout.maximumHeight: tunerView.maxHeight
            Layout.minimumHeight: tunerView.minHeight
        }

        Rectangle {
            id: statusBarRect
            color: "black"
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter

            Shape {
                id: topBorder
                ShapePath {
                    strokeWidth: 1
                    strokeColor: "gray"
                    fillRule: ShapePath.OddEvenFill
                    PathPolyline { path: [ Qt.point(0.0, 0.0), Qt.point(statusBarRect.width, 0.0) ] }
                }
            }

            StatusView {
                id: statusView
                anchors.fill: parent
            }
            implicitHeight: statusView.implicitHeight
        }
    }  // ColumnLayout
}
