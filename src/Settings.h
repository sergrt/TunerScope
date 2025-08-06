#pragma once

#include <QAudioDevice>
#include <QAbstractListModel>
#include <QList>

class Settings : public QAbstractListModel {
    Q_OBJECT

    enum Roles {
        DeviceName = Qt::UserRole + 21
    };

public:
    enum Channel {
        Left = 0,
        Right = 1,
        Both = 2
    };
    Q_PROPERTY(QString deviceName READ getDeviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(Channel channel READ getChannel WRITE setChannel NOTIFY channelChanged)
    Q_PROPERTY(int sampleRate READ getSampleRate WRITE setSampleRate NOTIFY sampleRateChanged)
    Q_PROPERTY(QAudioFormat::SampleFormat sampleFormat READ getSampleFormat WRITE setSampleFormat NOTIFY sampleFormatChanged)
    Q_PROPERTY(int fftSize READ getFftSize WRITE setFftSize NOTIFY fftSizeChanged)

    explicit Settings(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    QByteArray getDeviceId() const;
    QString getDeviceName() const;

    Channel getChannel() const;
    void setChannel(Channel channel);

    int getSampleRate() const;
    void setSampleRate(int sampleRate);

    QAudioFormat::SampleFormat getSampleFormat() const;
    void setSampleFormat(QAudioFormat::SampleFormat sampleFormat);

    int getFftSize() const;
    void setFftSize(int sz);

    int getRefreshRateMs() const;

signals:
    void deviceChanged(const QByteArray& id);
    void deviceNameChanged(const QString& deviceName);
    void channelChanged(Channel channel);
    void sampleRateChanged(int sampleRate);
    void sampleFormatChanged(QAudioFormat::SampleFormat sampleFormat);
    void fftSizeChanged(int value);

    void settingsChanged();

public slots:
    void handleFftSizeChange(int value);
    void changeDevice(int index);
    void save();

public:
    /*
    int fftSize =
    //256;
    //512;
    //1024;
    2048;
    //4096;
    //8192;
    //16384;
    //32768;
    //65536;
    //131072;
    //262144;
    */

private:
    void enumerateDevices();
    bool load();

    QList<QAudioDevice> m_devices;
    QByteArray m_deviceId{};
    QString m_deviceName;
    Channel m_channel{Channel::Left};
    int m_sampleRate{48000};
    QAudioFormat::SampleFormat m_sampleFormat{QAudioFormat::Float};
    int m_fftSize{4096};
    int m_refreshRateMs{50};
};
