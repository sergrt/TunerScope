#include "Settings.h"

#include <QDebug>
#include <QMediaDevices>

Settings::Settings(QObject *parent)
    : QAbstractListModel(parent) {

    enumerateDevices();
    if (!m_devices.empty()) {
        m_deviceId = m_devices[0].id();
        m_deviceName = m_devices[0].description();
    }
}

void Settings::enumerateDevices() {
    m_devices = QMediaDevices::audioInputs();
}

int Settings::rowCount(const QModelIndex &) const {
    return m_devices.size();
}

QVariant Settings::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return {};

    if (role == DeviceName) {
        return m_devices[index.row()].description();
    }

    return {};
}

QHash<int, QByteArray> Settings::roleNames() const {
    return {
        { DeviceName, "deviceName" }
    };
}

void Settings::changeDevice(int index) {
    m_deviceId = m_devices[index].id();
    m_deviceName = m_devices[index].description();
    emit deviceChanged(m_deviceId);
    emit deviceNameChanged(m_deviceName);
    emit settingsChanged();
}

QByteArray Settings::getDeviceId() const {
    return m_deviceId;
}

QString Settings::getDeviceName() const {
    return m_deviceName;
}

Settings::Channel Settings::getChannel() const {
    return m_channel;
}

void Settings::setChannel(Channel channel) {
    m_channel = channel;
    emit channelChanged(m_channel);
    emit settingsChanged();
}

int Settings::getSampleRate() const {
    return m_sampleRate;
}

void Settings::setSampleRate(int sampleRate) {
    m_sampleRate = sampleRate;
    emit sampleRateChanged(m_sampleRate);
    emit settingsChanged();
}

QAudioFormat::SampleFormat Settings::getSampleFormat() const {
    return m_sampleFormat;
}

void Settings::setSampleFormat(QAudioFormat::SampleFormat sampleFormat) {
    m_sampleFormat = sampleFormat;
    emit sampleFormatChanged(m_sampleFormat);
    emit settingsChanged();
}

int Settings::getFftSize() const {
    return m_fftSize;
}

void Settings::setFftSize(int sz) {
    m_fftSize = sz;
    emit fftSizeChanged(m_fftSize);
    emit settingsChanged();
}

int Settings::getRefreshRateMs() const {
    return m_refreshRateMs;
}

void Settings::handleFftSizeChange(int value) {
    m_fftSize = value;
    emit fftSizeChanged(m_fftSize);
    emit settingsChanged();
}
