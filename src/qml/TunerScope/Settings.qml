import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: settings
    anchors.centerIn: parent
    width: Math.min(parent.width / 2, 300)
    height: Math.min(parent.height / 1.2, settingsLayout.childrenRect.height + 2 * scrollView.anchors.margins)

    Rectangle {
        id: settingsWindow
        anchors.fill: parent
        color: "grey"
        border.color: "darkgrey"
        border.width: 2
        radius: 2

        ScrollView {
            id: scrollView
            anchors.fill: parent
            anchors.margins: 5

            ColumnLayout {
                id: settingsLayout
                width: scrollView.availableWidth

                Text {
                    text: qsTr("Audio device")
                    Layout.fillWidth: true
                }
                ComboBox {
                    Layout.fillWidth: true
                    model: settingsModel
                    Component.onCompleted: {
                        currentIndex = indexOfValue(settingsModel.deviceName);
                    }
                    onActivated: {
                        settingsModel.changeDevice(currentIndex);
                    }
                }

                Text {
                    text: qsTr("Channel(s)")
                }
                ComboBox {
                    Layout.fillWidth: true
                    textRole: "text"
                    valueRole: "value"
                    model: [
                        { value: 0, text: qsTr("Left") },
                        { value: 1, text: qsTr("Right") },
                        { value: 2, text: qsTr("Both (downmix to mono)") }
                    ]
                    currentIndex: settingsModel.channel
                    onActivated: {
                        settingsModel.channel = currentValue;
                        // INFO: for reference how to call c++ slot
                        //settingsModel.handleChannlsChange(currentValue);
                    }
                }

                Text {
                    text: qsTr("Sample rate")
                }
                ComboBox {
                    Layout.fillWidth: true
                    textRole: "text"
                    valueRole: "value"
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
                    Component.onCompleted: {
                        currentIndex = indexOfValue(settingsModel.sampleRate);
                    }
                    onActivated: {
                        settingsModel.sampleRate = currentValue;
                    }
                }

                Text {
                    text: qsTr("Sample format")
                }
                ComboBox {
                    Layout.fillWidth: true
                    textRole: "text"
                    valueRole: "value"
                    model: [
                        { value: 0 /*QAudioFormat.Unknown*/, text: "Default" },
                        { value: 1 /*QAudioFormat.UInt8*/, text: "UInt8" },
                        { value: 2 /*QAudioFormat.UInt16*/, text: "UInt16" },
                        { value: 3 /*QAudioFormat.Int32*/, text: "Int32" },
                        { value: 4 /*QAudioFormat.Float*/, text: "Float" },
                    ]
                    currentIndex: settingsModel.sampleFormat
                    onActivated: {
                        settingsModel.sampleFormat = currentValue;
                    }
                }

                Text {
                    text: qsTr("Fft size")
                }
                ComboBox {
                    Layout.fillWidth: true
                    textRole: "text"
                    valueRole: "value"
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
                    Component.onCompleted: {
                        currentIndex = indexOfValue(settingsModel.fftSize);
                    }
                    onActivated: {
                        settingsModel.fftSize = currentValue;
                    }
                }

                Button {
                    text: qsTr("Close")
                    onClicked: {
                        settings.visible = false;
                        mainLayout.visible = true;
                    }
                }
            }  // ColumnLayout
        }  // ScrollView

    }  // Rectangle
}
