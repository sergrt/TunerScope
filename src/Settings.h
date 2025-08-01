#pragma once

#include <QObject>

class Settings : public QObject {
    Q_OBJECT

public:
    Q_PROPERTY(int fftSize READ getFftSize WRITE setFftSize NOTIFY fftSizeChanged)

    explicit Settings(QObject *parent = nullptr);

    int getFftSize() const;
    void setFftSize(int sz);

signals:
    void fftSizeChanged(int value);

public slots:
    void handleFftSizeChange(int value);

public:
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
    int sampleRate = 48000;
    int refreshRateMs = 50;
};
