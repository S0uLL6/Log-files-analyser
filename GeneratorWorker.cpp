#include "GeneratorWorker.hpp"
#include <QDebug>

GeneratorWorker::GeneratorWorker(LazyWriteStream* stream, const QString& dictionaryFile, QObject *parent)
    : QObject(parent), generator(nullptr), writeStream(stream), dictionaryFile(dictionaryFile)
{
}

void GeneratorWorker::startGeneration() {
    if (!generator) {
        try {
            generator = new EventGenerator(*writeStream, dictionaryFile.toStdString());
            generator->Start();
            qDebug() << "GeneratorWorker: Generation started.";
        } catch (const std::exception& e) {
            emit error(QString::fromStdString("Failed to start generator: " + std::string(e.what())));
            return;
        }
    }
}

void GeneratorWorker::stopGeneration() {
    if (generator) {
        generator->Stop();
        generator->Join();
        delete generator;
        generator = nullptr;
        qDebug() << "GeneratorWorker: Generation stopped.";
        emit finished();
    }
}