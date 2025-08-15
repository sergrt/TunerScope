#pragma once

#include <QAbstractListModel>
#include <QAudioDevice>
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
    Q_PROPERTY(int wndWidth READ getWndWidth WRITE setWndWidth NOTIFY wndWidthChanged)
    Q_PROPERTY(int wndHeight READ getWndHeight WRITE setWndHeight NOTIFY wndHeightChanged)

    explicit Settings(QObject *parent = nullptr);
    ~Settings();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    QByteArray getDeviceId() const noexcept;
    QString getDeviceName() const noexcept;

    Channel getChannel() const noexcept;
    void setChannel(Channel channel);

    int getSampleRate() const noexcept;
    void setSampleRate(int sampleRate);

    QAudioFormat::SampleFormat getSampleFormat() const noexcept;
    void setSampleFormat(QAudioFormat::SampleFormat sampleFormat);

    int getFftSize() const noexcept;
    void setFftSize(int sz);

    int getRefreshRateMs() const noexcept;

    int getWndWidth() const noexcept;
    void setWndWidth(int w);

    int getWndHeight() const noexcept;
    void setWndHeight(int h);

signals:
    void deviceChanged(const QByteArray& id);
    void deviceNameChanged(const QString& deviceName);
    void channelChanged(Channel channel);
    void sampleRateChanged(int sampleRate);
    void sampleFormatChanged(QAudioFormat::SampleFormat sampleFormat);
    void fftSizeChanged(int value);
    void wndWidthChanged(int w);
    void wndHeightChanged(int h);

    void settingsChanged();

public slots:
    void handleFftSizeChange(int value);
    void changeDevice(int index);
    void save();

private:
    void enumerateDevices();
    bool load();

    const int kDefaultWndWidth{600};
    const int kDefaultWndHeight{400};

    QList<QAudioDevice> m_devices;
    QByteArray m_deviceId{};
    QString m_deviceName;
    Channel m_channel{Channel::Left};
    int m_sampleRate{48000};
    QAudioFormat::SampleFormat m_sampleFormat{QAudioFormat::Float};
    int m_fftSize{4096};
    int m_refreshRateMs{50};
    int m_wndWidth{kDefaultWndWidth};
    int m_wndHeight{kDefaultWndHeight};
};
