import QtQuick
import QtQuick.Window
import QtQuick.Controls

Popup {



    property alias text: toolTipText.text
contentItem:


       Text {
           id: toolTipText
           anchors.fill: parent
           anchors.margins: 5
           font.pixelSize: 12
           color: "black"
       }



}
