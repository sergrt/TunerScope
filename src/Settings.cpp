#include "Settings.h"

#include <QDebug>
#include <QThread>

Settings::Settings(QObject *parent)
    : QObject(parent) {

}

int Settings::getFftSize() const {
    return fftSize;
}

void Settings::setFftSize(int sz) {
    fftSize = sz;

    qDebug() << "setFftSize thread id = " << QThread::currentThreadId();
    emit fftSizeChanged(fftSize);
}

void Settings::handleFftSizeChange(int value) {
    fftSize = value;
    emit fftSizeChanged(value);
}
