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
                Layout.fillWidth: true
                model: settingsModel
                onActivated: {
                    settingsModel.changeDevice(currentIndex);
                }
            }

            Text {
                text: qsTr("Channel(s)")
            }
            ComboBox {
                Layout.fillWidth: true
                textRole: "text" // Role for display text
                valueRole: "value" // Role for the actual value to pass to C++
                model: [
                    { value: 0, text: "Left" },
                    { value: 1, text: "Right" },
                    { value: 2, text: "Both (downmix to mono)" }
                ]
                onActivated: {
                    // Update C++ property with the selected value
                    settingsModel.channel = currentValue;
                    // Or call a C++ slot
                    //settingsModel.handleFftSizeChange(currentValue);
                }
            }

            Text {
                text: qsTr("Sample rate")
            }
            ComboBox {
                Layout.fillWidth: true
                textRole: "text" // Role for display text
                valueRole: "value" // Role for the actual value to pass to C++
                model: [
                    { value: 8000, text: "8000" },
                    { value: 11025, text: "11025" },
                    { value: 16000, text: "16000" },
                    { value: 22050, text: "22050" },
                    { value: 44100, text: "44100" },
                    { value: 48000, text: "48000" },
                    { value: 88200, text: "88200" },
                    { value: 96000, text: "96000" },
                    { value: 176400, text: "176400" },
                    { value: 192000, text: "192000" },
                    { value: 352800, text: "352800" },
                    { value: 384000, text: "384000" }
                ]
                onActivated: {
                    // Update C++ property with the selected value
                    settingsModel.sampleRate = currentValue;
                    // Or call a C++ slot
                    //settingsModel.handleFftSizeChange(currentValue);
                }
            }

            Text {
                text: qsTr("Sample format")
            }
            ComboBox {
                Layout.fillWidth: true
                textRole: "text" // Role for display text
                valueRole: "value" // Role for the actual value to pass to C++
                model: [
                    { value: 0 /*QAudioFormat.Unknown*/, text: "Default" },
                    { value: 1 /*QAudioFormat.UInt8*/, text: "UInt8" },
                    { value: 2 /*QAudioFormat.UInt16*/, text: "UInt16" },
                    { value: 3 /*QAudioFormat.Int32*/, text: "Int32" },
                    { value: 4 /*QAudioFormat.Float*/, text: "Float" },
                ]
                onActivated: {
                    // Update C++ property with the selected value
                    settingsModel.sampleFormat = currentValue;
                    // Or call a C++ slot
                    //settingsModel.handleFftSizeChange(currentValue);
                }
            }

            Text {
                text: qsTr("Fft size")
            }
            ComboBox {
                Layout.fillWidth: true
                textRole: "text" // Role for display text
                valueRole: "value" // Role for the actual value to pass to C++
                model: [
                    { value: 256, text: "256" },
                    { value: 512, text: "512" },
                    { value: 1024, text: "1024" },
                    { value: 2048, text: "2048" },
                    { value: 4096, text: "4096" },
                    { value: 8192, text: "8192" },
                    { value: 16384, text: "16384" },
                    { value: 32768, text: "32768" },
                    { value: 65536, text: "65536" },
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
