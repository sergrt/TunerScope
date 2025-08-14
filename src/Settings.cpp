#include "Settings.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMediaDevices>

namespace {

static const char settingsFileName[] = "settings.json";

} // namespace

Settings::Settings(QObject *parent)
    : QAbstractListModel(parent) {

    QObject::connect(this, &Settings::settingsChanged, this, &Settings::save);

    enumerateDevices();

    if (!m_devices.empty()) {
        m_deviceId = m_devices[0].id();
        m_deviceName = m_devices[0].description();
    }

    if (!load())
        save();
}

Settings::~Settings() {
    try {
        save();
    } catch(...) {
        qWarning("Unable to save settings on exit");
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

int Settings::getWndWidth() const {
    return m_wndWidth;
}

void Settings::setWndWidth(int w) {
    m_wndWidth = w;
}

int Settings::getWndHeight() const {
    return m_wndHeight;
}

void Settings::setWndHeight(int h) {
    m_wndHeight = h;
}

void Settings::handleFftSizeChange(int value) {
    m_fftSize = value;

    emit fftSizeChanged(m_fftSize);
    emit settingsChanged();
}

bool Settings::load() {
    QFile file(settingsFileName);

    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray saveData = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(saveData));

    auto deviceName = doc["device_name"].toString();
    int i = 0;
    while(i < m_devices.size()) {
        const auto& device = m_devices[i];
        if (device.description() == deviceName) {
            m_deviceName = deviceName;
            m_deviceId = device.id();
            break;
        }
        ++i;
    }
    if (i == m_deviceName.size()) {
        qWarning("Device is missing in enumeration");
        return false;
    }

    m_channel = static_cast<Channel>(doc["channel"].toInt());
    m_sampleRate = doc["sample_rate"].toInt();
    m_sampleFormat = static_cast<QAudioFormat::SampleFormat>(doc["sample_format"].toInt());
    m_fftSize = doc["fft_size"].toInt();
    m_refreshRateMs = doc["refresh_rate_ms"].toInt();
    m_wndWidth = doc["wnd_width"].toInt(kDefaultWndWidth);
    m_wndHeight = doc["wnd_height"].toInt(kDefaultWndHeight);

    return true;
}

void Settings::save() {
    QFile file(settingsFileName);

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning("Unable to open file for saving");
        return;
    }

    QJsonObject saveObject;
    saveObject["device_name"] = m_deviceName;
    saveObject["channel"] = m_channel;
    saveObject["sample_rate"] = m_sampleRate;
    saveObject["sample_format"] = m_sampleFormat;
    saveObject["fft_size"] = m_fftSize;
    saveObject["refresh_rate_ms"] = m_refreshRateMs;
    saveObject["wnd_width"] = m_wndWidth;
    saveObject["wnd_height"] = m_wndHeight;

    QByteArray saveData = QJsonDocument(saveObject).toJson();
    file.write(saveData);
}
