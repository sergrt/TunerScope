import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes

Item {
    id: root
    anchors.fill: parent

    /*
    HoverHandler {
        id: hoverHandler
        cursorShape: Qt.CrossCursor
    }
    */

    LabelHelper {
        id: labelHelper
    }

    ListView {
        id: listRoot

        readonly property int markGapPx: 2 // vertical distance from scale mark to number
        readonly property int markHeightPx: 2
        readonly property int scaleHeight: fixedFontHeight + markGapPx + markHeightPx
        // readonly property int markDensity: 20 // Deprecated, used as index % markDensity === 0

        anchors.fill: parent
        orientation: ListView.Horizontal
        spacing: 0
        model: spectrumModel

        ScrollBar.horizontal:  ScrollBar {
            id: verticalScrollBar
            policy: ScrollBar.AlwaysOn
        }

        delegate: Item {
            id: delegate

            width: 2
            height: listRoot.height

            MouseArea {
                id: mouseArea

                anchors.fill: parent
                hoverEnabled: true // Enable hover events


                onEntered: {
                    var pos = mapToItem(listRoot, mouseX, mouseY)
                    currentFreqContainer.x = pos.x;
                    currentFreqContainer.text = scaleItem;
                    currentFreqContainer.visible = true;
                }

                onExited: {
                    if (currentFreqContainer.text == scaleItem)
                        currentFreqContainer.visible = false;
                }

                onPositionChanged: {
                    var pos = mapToItem(listRoot, mouseX, mouseY)
                    currentFreqContainer.x = pos.x;
                }
            }

            HoverHandler {
                id: hoverHandler
                /*
                onHoveredChanged: {
                    if (hovered) {
                        //cursorShape: Qt.CrossCursor
                    }
                }
                */
            }

            Rectangle {
                id: histogramItem
                width: parent.width
                height: Math.max(1, magnitude * (parent.height - listRoot.scaleHeight))
                y: parent.height - height - listRoot.scaleHeight
                color: hoverHandler.hovered ? "darkorange" : Qt.rgba(
                    magnitude,
                    0.4 + 0.6 * magnitude,
                    1.0,
                    1.0)
            }

            Shape {
                id: scaleMark

                visible: labelHelper.shouldDisplayLabel(index)

                ShapePath {
                    strokeWidth: 2
                    strokeColor: "white"
                    fillColor: "orange"
                    fillRule: ShapePath.OddEvenFill

                    PathPolyline {
                            path: [ Qt.point(0.0, histogramItem.y + histogramItem.height + 1),
                                    Qt.point(0.0, histogramItem.y + histogramItem.height + 1 + listRoot.markHeightPx) ]
                    }
                }
            }

            Text {
                id: scaleText
                //visible: index % listRoot.markDensity == 0
                visible: labelHelper.shouldDisplayLabel(index)
                x:  - width / 2
                y: parent.height - listRoot.scaleHeight - 2 + listRoot.markGapPx + listRoot.markHeightPx

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font: fixedFont
                text: scaleItem
                color: "white"
            }
        } // delegate

        Rectangle {
            id: currentFreqContainer

            property alias text: currentFreq.text

            border.color: "dimgrey"
            color: "antiquewhite"
            border.width: 1
            radius: 3
            width: currentFreq.width + 2 * radius
            height: currentFreq.height + 2 * radius
            visible: false

            Text {
                id: currentFreq

                anchors.centerIn: parent
                font: fixedFont
                color: "black"
                fontSizeMode: Text.Fit
            }
        }
    }
}

