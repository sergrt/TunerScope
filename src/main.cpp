#include "AudioEngine.h"
#include "Settings.h"
#include "SpectrumModel.h"
#include "TunerModel.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFontDatabase>
#include <QFontMetrics>

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

    Settings settings{};
    AudioEngine audioEngine{};
    audioEngine.updateFromSettings(settings);
    SpectrumModel spectrumModel;
    spectrumModel.updateFromSettings(settings);
    TunerModel tunerModel;
    tunerModel.updateFromSettings(settings);

    QObject::connect(&audioEngine, &AudioEngine::spectrumUpdated,
                     &spectrumModel, &SpectrumModel::updateSpectrum);
    QObject::connect(&audioEngine, &AudioEngine::spectrumUpdatedWithData,
                     &tunerModel, &TunerModel::updateDetectedNotes);

    QObject::connect(&settings, &Settings::settingsChanged,
                     &audioEngine, [&settings, &audioEngine] {
                        audioEngine.updateFromSettings(settings);
                        audioEngine.restart();
                    });

    QObject::connect(&settings, &Settings::settingsChanged,
                     &spectrumModel, [&settings, &spectrumModel] {
                        spectrumModel.updateFromSettings(settings);
                    });

    QObject::connect(&settings, &Settings::settingsChanged,
                     &tunerModel, [&settings, &tunerModel] {
                         tunerModel.updateFromSettings(settings);
                     });

    engine.rootContext()->setContextProperty("settingsModel", &settings);
    engine.rootContext()->setContextProperty("audioEngine", &audioEngine);
    engine.rootContext()->setContextProperty("spectrumModel", &spectrumModel);
    engine.rootContext()->setContextProperty("tunerModel", &tunerModel);

    QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont);
    engine.rootContext()->setContextProperty("fixedFont", fixedFont);
    const auto fixedFontHeight = 2 * QFontMetrics(fixedFont).height();
    engine.rootContext()->setContextProperty("fixedFontHeight", fixedFontHeight);

    audioEngine.start();

    engine.load(QUrl(QStringLiteral("qrc:///qml/TunerScope/main.qml")));

    if (engine.rootObjects().isEmpty()) {
        qCritical("Unable to start engine");
        return -1;
    }

    return app.exec();
}
