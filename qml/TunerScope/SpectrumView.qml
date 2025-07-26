import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

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

            ToolTip.visible: hoverHandler.hovered

            width: 2
            height: list_root.height

            //ToolTip.visible: hovered
            ToolTip.text: tooltip
            ToolTip.delay: 0




            Rectangle {
                width: parent.width
                height: Math.max(1, magnitude * parent.height)
                y: parent.height- height  // ← растёт снизу вверх
                    //list_root.height - 30// - height
                color: Qt.rgba(
                    magnitude,
                    0.4 + 0.6 * magnitude,
                    1.0,
                    1.0
                )
            }
        }

    }


}
