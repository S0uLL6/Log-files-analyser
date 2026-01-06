#ifndef GENERATORWORKER_HPP
#define GENERATORWORKER_HPP

#include <QObject>
#include "EventGenerator.hpp"
#include "LazyWriteStream.hpp"

class GeneratorWorker : public QObject
{
    Q_OBJECT

public:
    explicit GeneratorWorker(LazyWriteStream* stream, const QString& dictionaryFile, QObject *parent = nullptr);

public slots:
    void startGeneration();
    void stopGeneration();

signals:
    void finished();
    void error(const QString& message);

private:
    EventGenerator* generator;
    LazyWriteStream* writeStream;
    QString dictionaryFile;
};

#endif // GENERATORWORKER_HPP