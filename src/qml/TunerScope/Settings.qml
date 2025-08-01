import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: settings
    anchors.centerIn: parent
    width: 300
    height: 300

    /*
    SettingsModel {
        id: settingsModel
         // Properties and slots from your C++ class
    }
    */

    Rectangle {
        anchors.fill: parent
        color: "grey"

        ColumnLayout {
            anchors.fill: parent


            Text {
                text: qsTr("Audio device")
            }
            ComboBox {

            }

            Text {
                text: qsTr("Channel(s)")
            }
            ComboBox {

            }

            Text {
                text: qsTr("Sample rate")
            }
            TextInput {
                text: "48000"
            }

            Text {
                text: qsTr("Fft size")
            }
            ComboBox {
                textRole: "text" // Role for display text
                valueRole: "value" // Role for the actual value to pass to C++
                model: [
                    { value: 256, text: "256" },
                    { value: 512, text: "512" },
                    { value: 1024, text: "1024" }
                ]
                onActivated: {
                    // Update C++ property with the selected value
                    settingsModel.fftSize = currentValue;
                    // Or call a C++ slot
                    //settingsModel.handleFftSizeChange(currentValue);
                }
            }

            Button {
                text: qsTr("Close")
                onClicked: {
                    settings.visible = false;
                    mainLayout.visible = true;
                }
            }
        }

    }
}
