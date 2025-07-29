#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFontDatabase>
#include <QFontMetrics>

#include "AudioEngine.h"
#include "Settings.h"
#include "SpectrumModel.h"
#include "TunerModel.h"


int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

    Settings settings{};

    AudioEngine audioEngine;
    audioEngine.Start(settings);

    SpectrumModel spectrumModel(settings);
    TunerModel tunerModel(settings);

    QObject::connect(&audioEngine, &AudioEngine::spectrumUpdated,
                     &spectrumModel, &SpectrumModel::updateSpectrum);
    QObject::connect(&audioEngine, &AudioEngine::spectrumUpdated,
                     &tunerModel, &TunerModel::updateDetectedNotes);

    //qmlRegisterType<SpectrumModel>("TunerScope", 1, 0, "SpectrumModel");

    engine.rootContext()->setContextProperty("audioEngine", &audioEngine);
    engine.rootContext()->setContextProperty("spectrumModel", &spectrumModel);
    engine.rootContext()->setContextProperty("tunerModel", &tunerModel);

    QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont);
    engine.rootContext()->setContextProperty("fixedFont", fixedFont);
    auto fixedFontHeight = 2 * QFontMetrics(fixedFont).height();
    engine.rootContext()->setContextProperty("fixedFontHeight", fixedFontHeight);

    //engine.loadFromModule("TunerScope", "main");
    engine.load(QUrl(QStringLiteral("TunerScope/qml/TunerScope/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
