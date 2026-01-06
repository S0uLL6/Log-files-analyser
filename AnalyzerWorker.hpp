#ifndef ANALYZERWORKER_HPP
#define ANALYZERWORKER_HPP

#include <QObject>
#include "EventAnalyzer.hpp"
#include "LazyReadStream.hpp"

class AnalyzerWorker : public QObject
{
    Q_OBJECT

public:
    explicit AnalyzerWorker(LazyReadStream* stream, QObject *parent = nullptr);

public slots:
    void startAnalysisAll();
    void startAnalysisTagsOnly();
    void startAnalysisNonTagsOnly();
    void stopAnalysis();

signals:
    void finished();
    void error(const QString& message);

private:
    EventAnalyzer* analyzer;
    LazyReadStream* readStream;
};

#endif // ANALYZERWORKER_HPP