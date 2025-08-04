#include "Settings.h"

#include <QDebug>
#include <QMediaDevices>

Settings::Settings(QObject *parent)
    : QAbstractListModel(parent) {

    enumerateDevices();
}

void Settings::enumerateDevices() {
    devices_ = QMediaDevices::audioInputs();
}

int Settings::rowCount(const QModelIndex &) const {
    return devices_.size();
}

QVariant Settings::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return {};

    if (role == DeviceName) {
        return devices_[index.row()].description();
    }

    return {};
}

QHash<int, QByteArray> Settings::roleNames() const {
    return {
        { DeviceName, "deviceName" }
    };
}

void Settings::changeDevice(int index) {
    deviceId_ = devices_[index].id();
    emit deviceChanged(deviceId_);
    emit settingsChanged();
}

QByteArray Settings::getDevieId() const {
    return deviceId_;
}

Settings::Channel Settings::getChannel() const {
    return channel_;
}

void Settings::setChannel(Channel channel) {
    channel_ = channel;
    emit channelChanged(channel_);
    emit settingsChanged();
}

int Settings::getSampleRate() const {
    return sampleRate_;
}

void Settings::setSampleRate(int sampleRate) {
    sampleRate_ = sampleRate;
    emit sampleRateChanged(sampleRate_);
    emit settingsChanged();
}

QAudioFormat::SampleFormat Settings::getSampleFormat() const {
    return sampleFormat_;
}

void Settings::setSampleFormat(QAudioFormat::SampleFormat sampleFormat) {
    sampleFormat_ = sampleFormat;
    emit sampleFormatChanged(sampleFormat_);
    emit settingsChanged();
}

int Settings::getFftSize() const {
    return fftSize_;
}

void Settings::setFftSize(int sz) {
    fftSize_ = sz;
    emit fftSizeChanged(fftSize_);
    emit settingsChanged();
}

int Settings::getRefreshRateMs() const {
    return refreshRateMs_;
}

void Settings::handleFftSizeChange(int value) {
    fftSize_ = value;
    emit fftSizeChanged(fftSize_);
    emit settingsChanged();
}
