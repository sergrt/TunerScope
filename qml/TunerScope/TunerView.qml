import QtQuick

Item {
    // Placeholder for future tuner UI
    // Currently not used in main.qml, but reserved for pitch indicator etc.
    Text {
        id: max_freq
        anchors.centerIn: parent
        color: "green"
        font.pixelSize: 30

    }

    Connections {
        target: tunerModel // Assuming 'myCppObject' is a C++ object exposed to QML
        function onUpdateNote(noteName, noteFreq, curFreq, cents) { // 'myCppSignal' is a signal defined in your C++ class
            //console.log("C++ signal received!");
            max_freq.text = noteName + " (" + (noteFreq).toFixed(3) + ") " + (curFreq).toFixed(3) + " (" + Math.round(cents) + ")"
        }
    }
}
