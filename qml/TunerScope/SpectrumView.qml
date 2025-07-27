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
            onHoveredChanged: {
                //if (hovered)
                //    toolTip.show()
                //cursorShape: Qt.CrossCursor
            }
        }
*/




    ListView {



        id: list_root
        anchors.fill: parent
        //implicitHeight: 100
        //width: parent.width
        //height: parent.height

        ScrollBar.horizontal:  ScrollBar {
                    id: verticalScrollBar
                    policy: ScrollBar.AlwaysOn // Or ScrollBar.AsNeeded
                }

        orientation: ListView.Horizontal
        spacing: 0
        model: spectrumModel
        readonly property int mark_gap_px: 2
        readonly property int mark_height_px: 2
        readonly property int scale_height: fixedFontHeight + mark_gap_px + mark_height_px
        readonly property int mark_density: 20



        /*
        delegate: Rectangle {
                width: 2
                height: Math.max(1, magnitude * list_root.height)
                y: list_root.height - height

                // Цвет от синего (низкая амплитуда) до жёлтого/белого (высокая)
                color: Qt.rgba(
                    magnitude,                      // R – от тёмного к яркому
                    0.3 + 0.7 * magnitude,          // G – мягкий градиент
                    1.0,                            // B – всегда синий
                    1.0
                )
            }
        */




        delegate: Item {
            id: delegate
            /*
            MouseArea {
                        id: _mouseArea
                        hoverEnabled: true
                        anchors.fill: parent
                        cursorShape: Qt.CrossCursor
                        //onHoveredChanged: {
                        //if (hovered)
                        //    toolTip.show()
                        //    cursorShape: Qt.CrossCursor
                        //}
                    }
            ToolTip.visible: _mouseArea.containsMouse
            */
            HoverHandler {
                    id: hoverHandler
                    //cursorShape: Qt.CrossCursor
                    onHoveredChanged: {

                        if (hovered) {
                            //delegate.ToolTip.show("1")
                            //cursorShape: Qt.CrossCursor
                        }
                    }
                }

            /*
            ToolTip.visible: hoverHandler.hovered
            ToolTip.text: tooltip
            ToolTip.delay: 0
            */
            width: 2
            height: list_root.height



            CustomTooltip {
                        id: myCustomToolTip
                    }




MouseArea {
    id: area
            Rectangle {
                id: gistogram_item
                width: parent.width
                height: Math.max(1, magnitude * (parent.height - list_root.scale_height))
                y: parent.height- height - list_root.scale_height// ← растёт снизу вверх
                    //list_root.height - 30// - height
                color:
                    hoverHandler.hovered ? "darkorange" :

                                           Qt.rgba(
                    magnitude,
                    0.4 + 0.6 * magnitude,
                    1.0,
                    1.0
                )
            }



            /*
            Shape {
                    //anchors.fill: parent
                    //anchors.centerIn: parent
                id: mark_current
                //readonly property int mark_height: 2
                visible: index % list_root.mark_density == 0
                    ShapePath {
                        strokeWidth: 2
                        strokeColor: "orange"
                        fillColor: "orange"
                        fillRule: ShapePath.OddEvenFill

                        PathPolyline {
                            path: [ Qt.point(0.0, 0.0),
                                        Qt.point(0.0, gistogram_item.y + gistogram_item.height),

                                      ]

                        }
                    }
                }
                */



                                anchors.fill: parent
                                hoverEnabled: true // Enable hover events


                                onEntered: {
                                    // Show the tooltip at the mouse cursor position relative to the window
                                    //var pos = mapToItem(list_root, mouseX, mouseY)
                                    //pos.x = pos.x - list_root.x
                                    //pos.y = pos.y - list_root.y
                                    //pos = mapFromItem(list_root, pos.x, pos.y)
                                    //myCustomToolTip.show(mouseX + 10, mouseY + 10, "This is a custom tooltip!")
                                    //myCustomToolTip.show(pos.x, pos.y, "This is a custom tooltip!")

                                    //myCustomToolTip.show(mouseX, mouseY + gistogram_item.height, scaleItem)
                                    myCustomToolTip.contentItem.text = scaleItem
                                    myCustomToolTip.x = mouseX + 0
                                    //myCustomToolTip.x = mouseX
                                    //myCustomToolTip.y = mouseY + gistogram_item.y
                                    //myCustomToolTip.open()

                                    var pos = mapToItem(list_root, mouseX, mouseY)
                                    current_freq_container.x = pos.x;
                                    current_freq_container.text = scaleItem;
                                    current_freq_container.visible = true;
                                    //console.log("+");


                                }

                                onExited: {
                                    //myCustomToolTip.hide()
                                    //console.log("1")
                                    //myCustomToolTip.close()
                                    if (current_freq_container.text == scaleItem)
                                        current_freq_container.visible = false;
                                    //console.log("-");
                                }

                                onPositionChanged: {
                                    // Update tooltip position as the mouse moves within the area
                                    if (myCustomToolTip.visible) {
                                        //var pos = mapToItem(list_root, mouseX, mouseY)
                                        //pos.x = pos.x - list_root.x
                                        //pos.y = pos.y - list_root.y
                                        //pos = mapFromItem(list_root, pos.x, pos.y)
                                        //myCustomToolTip.x = pos.x
                                        //myCustomToolTip.y = pos.y
                                        //myCustomToolTip.x = mouseX
                                        //myCustomToolTip.y = mouseY + gistogram_item.y
                                        myCustomToolTip.x = mouseX + 0
                                        //myCustomToolTip.text = scaleItem
                                        //myCustomToolTip.x = mouseX
                                        //myCustomToolTip.y = mouseY + gistogram_item.y
                                        var pos = mapToItem(list_root, mouseX, mouseY)
                                        current_freq_container.x = pos.x;
                                    }
                                }



            }


            Shape {
                    //anchors.fill: parent
                    //anchors.centerIn: parent
                id: mark
                //readonly property int mark_height: 2
                visible: index % list_root.mark_density == 0
                    ShapePath {
                        strokeWidth: 2
                        strokeColor: "white"
                        fillColor: "orange"
                        fillRule: ShapePath.OddEvenFill

                        PathPolyline {
                            path: [ Qt.point(0.0, gistogram_item.y + gistogram_item.height + 1),
                                        Qt.point(0.0, gistogram_item.y + gistogram_item.height + 1 + list_root.mark_height_px),

                                      ]

                        }
                    }
                }





            Text {
                visible: index % list_root.mark_density == 0
                y: parent.height - list_root.scale_height - 2 + list_root.mark_gap_px + list_root.mark_height_px
                x:  - width / 2
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                //anchors.horizontalCenter: gistogram_item
                //fontSizeMode: Text.Fit
                font: fixedFont

                //width: 10
                text: scaleItem
                color: "white"
            }







        }


        Rectangle {
            id: current_freq_container
            property alias text: current_freq.text

            //anchors.left: gistogram_item.left
            //anchors.top: gistogram_item.top

            border.color: "dimgrey"
            color: "antiquewhite"
            border.width: 1
            radius: 3

            width: current_freq.width + 2 * radius
            height: current_freq.height + 2 * radius

            visible: false

            Text {

                anchors.centerIn: parent
                id: current_freq
                //readonly property int mark_height: 2
                visible: true
                font: fixedFont

                //width: 10
                text: "123"//scaleItem
                color: "black"
                y: 0
                fontSizeMode: Text.Fit
                //width: 50
                //x: 0
                //z: 1
            }
        }
    }


}
