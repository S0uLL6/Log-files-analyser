#include "AnalyzerWorker.hpp"
#include <QDebug>

AnalyzerWorker::AnalyzerWorker(LazyReadStream* stream, QObject *parent)
    : QObject(parent), analyzer(new EventAnalyzer(*stream)), readStream(stream)
{
}

void AnalyzerWorker::startAnalysisAll() {
    analyzer->Start();
    qDebug() << "AnalyzerWorker: Analysis (All) started.";
}

void AnalyzerWorker::startAnalysisTagsOnly() {
    analyzer->StartAnalysisByTagsOnly();
    qDebug() << "AnalyzerWorker: Analysis (Tags Only) started.";
}

void AnalyzerWorker::startAnalysisNonTagsOnly() {
    analyzer->StartAnalysisByNonTagsOnly();
    qDebug() << "AnalyzerWorker: Analysis (Non-Tags Only) started.";
}

void AnalyzerWorker::stopAnalysis() {
    // analyzer->Stop(); // Устанавливает флаг
    // analyzer->Join(); // Ждет завершения
    // Теперь MainWindow закрывает поток, *затем* вызывает stopAnalysis
    // analyzer->Stop() устанавливает флаг
    analyzer->Stop();
    // analyzer->Join() ждет, пока цикл анализа (который проверяет флаг и читает из потока) не завершится
    // Так как MainWindow закрыл writeStream, Read() в цикле бросит EndOfStreamException
    // Цикл проверит флаг (он установлен) и выйдет
    // analyzer->Join() завершится
    analyzer->Join();
    qDebug() << "AnalyzerWorker: Analysis stopped.";
    emit finished();
}