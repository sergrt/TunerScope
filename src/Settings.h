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

    Q_PROPERTY(Channel m_channel READ getChannel WRITE setChannel NOTIFY channelChanged)
    Q_PROPERTY(int m_sampleRate READ getSampleRate WRITE setSampleRate NOTIFY sampleRateChanged)
    Q_PROPERTY(QAudioFormat::SampleFormat m_sampleFormat READ getSampleFormat WRITE setSampleFormat NOTIFY sampleFormatChanged)
    Q_PROPERTY(int m_fftSize READ getFftSize WRITE setFftSize NOTIFY fftSizeChanged)

    explicit Settings(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    QByteArray getDevieId() const;

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
    void channelChanged(Channel channel);
    void sampleRateChanged(int sampleRate);
    void sampleFormatChanged(QAudioFormat::SampleFormat sampleFormat);
    void fftSizeChanged(int value);

    void settingsChanged();

public slots:
    void handleFftSizeChange(int value);
    void changeDevice(int index);

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

    QList<QAudioDevice> devices_;
    QByteArray deviceId_{};
    Channel channel_{Channel::Left};
    int sampleRate_{48000};
    QAudioFormat::SampleFormat sampleFormat_{QAudioFormat::Float};
    int fftSize_{4096};
    int refreshRateMs_{50};
};
