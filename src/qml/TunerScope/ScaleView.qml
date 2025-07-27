import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes

Item {
    id: root
    anchors.fill: parent

    ListView {
        id: list_root
        anchors.fill: parent
        //implicitHeight: 100


        orientation: ListView.Horizontal
        spacing: 0
        model: scaleModel
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
            width: 2
            height: list_root.height

            visible: index % 22 == 0
            Rectangle {
                /*
                Shape {
                        anchors.fill: parent
                        anchors.centerIn: parent

                        ShapePath {
                            strokeWidth: 1
                            strokeColor: "red"
                            fillColor: "orange"
                            fillRule: ShapePath.OddEvenFill

                            PathPolyline {
                                path: [ Qt.point(0.0, 0.0),
                                            Qt.point(0.0, parent.height),

                                          ]

                            }
                        }
                    }
                    */


                Text {
                    //anchors: parent
                    width: parent.width
                    text: scale_item
                    color: "green"
                }

            }
        }
    }

}
