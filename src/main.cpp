#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "AudioEngine.h"
#include "SpectrumModel.h"
#include "ScaleModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);


    AudioEngine audioEngine;
    SpectrumModel spectrumModel;
    ScaleModel scaleModel;

    QObject::connect(&audioEngine, &AudioEngine::spectrumUpdated,
                     &spectrumModel, &SpectrumModel::updateSpectrum);
    QObject::connect(&audioEngine, &AudioEngine::spectrumUpdated,
                     &scaleModel, &ScaleModel::updateScale);

    qmlRegisterType<SpectrumModel>("TunerScope", 1, 0, "SpectrumModel");

    engine.rootContext()->setContextProperty("audioEngine", &audioEngine);
    engine.rootContext()->setContextProperty("spectrumModel", &spectrumModel);
    engine.rootContext()->setContextProperty("scaleModel", &scaleModel);

    //engine.addImportPath(":/qt/qml/");
    //engine.loadFromModule("TunerScope", "main");
    engine.load(QUrl(QStringLiteral("TunerScope/qml/TunerScope/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
