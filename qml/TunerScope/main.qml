import QtQuick
import QtQuick.Window
//import TunerScope
import QtQuick.Layouts

Window {
    width: 640
    height: 360
    visible: true
    title: qsTr("TunerScope")



    ColumnLayout {
        anchors.fill: parent
        spacing: 20
        Rectangle {
            //anchors.fill: parent
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
            Layout.fillHeight: false
            Layout.fillWidth: true
            Layout.alignment: top
            implicitHeight: 2 * parent.height / 3
            //ScaleView {
            //    anchors.fill: parent
            //}


        }


    }
}
