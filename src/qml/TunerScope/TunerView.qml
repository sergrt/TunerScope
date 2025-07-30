import QtQuick

Item {
    id: root
    anchors.fill: parent

    ListView {
        id: closestNotesView
        anchors.fill: parent
        orientation: ListView.Horizontal
        spacing: 0
        model: tunerModel

        delegate: Item {
            id: noteDelegate
            height: closestNotesView.height
            width: closestNotesView.width / 3 //(closestNotesView.width - 2 * closestNotesView.spacing) / 3
            y: 20 // TODO: make font as context item and calculate center properly

            Text {
                id: noteText
                anchors.horizontalCenter: parent.horizontalCenter
                color: "gray"
                font.pixelSize: 33
                font.bold: true
                text: noteName
            }

            Text {
                id: deviationText
                anchors.horizontalCenter: parent.horizontalCenter
                color: {
                    const absCents = Math.abs(cents)
                    const t = Math.min(absCents / 50.0, 1.0)  // normalize to [0:1] (max 50 cents)
                    const r = Math.round(255 * t)
                    const g = Math.round(255 * (1.0 - t))
                    return Qt.rgba(r / 255, g / 255, 0, 1.0)
                }

                y: noteText.y + 38
                font.pixelSize: 28
                font.bold: true
                text: Math.round(cents)

                visible: index === 1
            }

            Text {
                id: noteFreqText
                anchors.horizontalCenter: parent.horizontalCenter
                color: "green"
                y: deviationText.y + 36
                font.pixelSize: 16
                text: (noteFreq).toFixed(1) + (index === 1 ? " (" + (curFreq).toFixed(1) + ")" : "")
            }
        }
    }
}
