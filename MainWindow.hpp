#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QPushButton>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTextEdit>
#include <QLabel>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QTabWidget>
#include <QListWidget>
#include <QThread>
#include "LazyWriteStream.hpp"
#include "LazyReadStream.hpp"
#include "GeneratorWorker.hpp"
#include "AnalyzerWorker.hpp"

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStartGeneratorClicked();
    void onStopGeneratorClicked();
    void onStartAnalysisAllClicked();
    void onStartAnalysisTagsClicked();
    void onStartAnalysisNonTagsClicked();
    void onStopAnalysisClicked();
    void onLoadTopEventsClicked();
    void onLoadIsolatedEventsClicked();

    void onGeneratorFinished();
    void onGeneratorError(const QString& message);
    void onAnalyzerFinished();
    void onAnalyzerError(const QString& message);

    void onGraphVertexClicked(const QString& eventId);

private:
    void setupUI();
    void updateStatus();

    LazyWriteStream* writeStream;
    LazyReadStream* readStream;

    QThread* generatorThread;
    QThread* analyzerThread;
    GeneratorWorker* generatorWorker;
    AnalyzerWorker* analyzerWorker;

    QGraphicsView* graphView;
    QGraphicsScene* graphScene;
    QTextEdit* detailsTextEdit;
    QStatusBar* statusBar;
    QPushButton* startGeneratorButton;
    QPushButton* stopGeneratorButton;
    QPushButton* startAnalysisAllButton;
    QPushButton* startAnalysisTagsButton;
    QPushButton* startAnalysisNonTagsButton;
    QPushButton* stopAnalysisButton;
    QPushButton* loadTopEventsButton;
    QPushButton* loadIsolatedEventsButton;
    QListWidget* topEventsList;
    QListWidget* isolatedEventsList;

    bool isGeneratorRunning;
    bool isAnalyzerRunning;
};

#endif // MAINWINDOW_HPP