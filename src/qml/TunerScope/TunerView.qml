import QtQuick

Item {
    Text {
        id: max_freq
        anchors.centerIn: parent
        color: "green"
        font.pixelSize: 30
    }

    Connections {
        target: tunerModel
        function onUpdateNote(noteName, noteFreq, curFreq, cents) {
            max_freq.text = noteName + " (" + (noteFreq).toFixed(3) + ") " + (curFreq).toFixed(3) + " (" + Math.round(cents) + ")"
        }
    }
}
