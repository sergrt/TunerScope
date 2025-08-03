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

    Settings settings;

    AudioEngine audioEngine;
    audioEngine.UpdateSettings(settings);
    audioEngine.Start();

    SpectrumModel spectrumModel;
    spectrumModel.UpdateSettings(settings);
    TunerModel tunerModel;
    tunerModel.UpdateSettings(settings);

    QObject::connect(&audioEngine, &AudioEngine::spectrumUpdated,
                     &spectrumModel, &SpectrumModel::updateSpectrum);
    QObject::connect(&audioEngine, &AudioEngine::spectrumUpdated,
                     &tunerModel, &TunerModel::updateDetectedNotes);

    QObject::connect(&settings, &Settings::settingsChanged,
                     &audioEngine, [&settings, &audioEngine] {
                        audioEngine.UpdateSettings(settings);
                        audioEngine.restart();
                    });

    QObject::connect(&settings, &Settings::settingsChanged,
                     &spectrumModel, [&settings, &spectrumModel] {
                        spectrumModel.UpdateSettings(settings);
                    });
    QObject::connect(&settings, &Settings::settingsChanged,
                     &tunerModel, [&settings, &tunerModel] {
                         tunerModel.UpdateSettings(settings);
                     });


    //qmlRegisterType<Settings>("TunerScope", 1, 0, "SettingsModel");
    engine.rootContext()->setContextProperty("settingsModel", &settings);

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
