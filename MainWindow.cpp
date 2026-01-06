#include "MainWindow.hpp"
#include <QApplication>
#include <QDebug>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      writeStream(new LazyWriteStream()),
      readStream(new LazyReadStream(*writeStream)),
      generatorThread(new QThread(this)),
      analyzerThread(new QThread(this)),
      generatorWorker(new GeneratorWorker(writeStream, "dictionary.json")),
      analyzerWorker(new AnalyzerWorker(readStream)),
      isGeneratorRunning(false),
      isAnalyzerRunning(false)
{
    generatorWorker->moveToThread(generatorThread);
    analyzerWorker->moveToThread(analyzerThread);

    // Generator
    connect(generatorThread, &QThread::started, generatorWorker, &GeneratorWorker::startGeneration);
    connect(generatorWorker, &GeneratorWorker::finished, generatorThread, &QThread::quit);
    connect(generatorWorker, &GeneratorWorker::finished, generatorWorker, &GeneratorWorker::deleteLater);
    connect(generatorThread, &QThread::finished, generatorThread, &QThread::deleteLater);

    // Analyzer
    // Сигнал started не используется напрямую, вызов startAnalysis... происходит в MainWindow
    connect(analyzerWorker, &AnalyzerWorker::finished, analyzerThread, &QThread::quit);
    connect(analyzerWorker, &AnalyzerWorker::finished, analyzerWorker, &AnalyzerWorker::deleteLater);
    connect(analyzerThread, &QThread::finished, analyzerThread, &QThread::deleteLater);

    connect(generatorWorker, &GeneratorWorker::finished, this, &MainWindow::onGeneratorFinished);
    connect(generatorWorker, &GeneratorWorker::error, this, &MainWindow::onGeneratorError);
    connect(analyzerWorker, &AnalyzerWorker::finished, this, &MainWindow::onAnalyzerFinished);
    connect(analyzerWorker, &AnalyzerWorker::error, this, &MainWindow::onAnalyzerError);

    setupUI();
    updateStatus();
}

MainWindow::~MainWindow() {
    // Попробуем корректно остановить потоки
    if (isGeneratorRunning) {
        writeStream->Close();
        generatorThread->quit();
        generatorThread->wait();
    }
    if (isAnalyzerRunning) {
        writeStream->Close(); // Закрываем поток, чтобы прервать анализатор
        analyzerThread->quit();
        analyzerThread->wait();
    }
    delete writeStream;
    delete readStream;
}

void MainWindow::setupUI() {
    setWindowTitle("Log Files Analyzer");
    resize(1200, 800);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    QGroupBox* controlGroup = new QGroupBox("Control Panel");
    QHBoxLayout* controlLayout = new QHBoxLayout(controlGroup);

    startGeneratorButton = new QPushButton("Start Generator");
    stopGeneratorButton = new QPushButton("Stop Generator");
    startAnalysisAllButton = new QPushButton("Start Analysis (All)");
    startAnalysisTagsButton = new QPushButton("Start Analysis (Tags Only)");
    startAnalysisNonTagsButton = new QPushButton("Start Analysis (Non-Tags Only)");
    stopAnalysisButton = new QPushButton("Stop Analysis");

    controlLayout->addWidget(startGeneratorButton);
    controlLayout->addWidget(stopGeneratorButton);
    controlLayout->addWidget(startAnalysisAllButton);
    controlLayout->addWidget(startAnalysisTagsButton);
    controlLayout->addWidget(startAnalysisNonTagsButton);
    controlLayout->addWidget(stopAnalysisButton);

    mainLayout->addWidget(controlGroup);

    QHBoxLayout* mainContentLayout = new QHBoxLayout();

    graphView = new QGraphicsView();
    graphScene = new QGraphicsScene();
    graphView->setScene(graphScene);
    graphView->setRenderHint(QPainter::Antialiasing);
    mainContentLayout->addWidget(graphView, 3);

    QVBoxLayout* rightPanelLayout = new QVBoxLayout();

    detailsTextEdit = new QTextEdit();
    detailsTextEdit->setReadOnly(true);
    detailsTextEdit->setPlaceholderText("Click on a graph vertex to see event details.");
    rightPanelLayout->addWidget(new QLabel("Event Details:"));
    rightPanelLayout->addWidget(detailsTextEdit, 2);

    QTabWidget* statsTabs = new QTabWidget();
    topEventsList = new QListWidget();
    isolatedEventsList = new QListWidget();
    statsTabs->addTab(topEventsList, "Top Connected Events");
    statsTabs->addTab(isolatedEventsList, "Isolated Events");

    QHBoxLayout* statsButtonLayout = new QHBoxLayout();
    loadTopEventsButton = new QPushButton("Load Top Events");
    loadIsolatedEventsButton = new QPushButton("Load Isolated Events");
    statsButtonLayout->addWidget(loadTopEventsButton);
    statsButtonLayout->addWidget(loadIsolatedEventsButton);

    QVBoxLayout* statsLayout = new QVBoxLayout();
    statsLayout->addLayout(statsButtonLayout);
    statsLayout->addWidget(statsTabs);

    QWidget* statsWidget = new QWidget();
    statsWidget->setLayout(statsLayout);
    rightPanelLayout->addWidget(statsWidget, 1);

    mainContentLayout->addLayout(rightPanelLayout, 1);
    mainLayout->addLayout(mainContentLayout, 1);

    statusBar = new QStatusBar();
    setStatusBar(statusBar);

    connect(startGeneratorButton, &QPushButton::clicked, this, &MainWindow::onStartGeneratorClicked);
    connect(stopGeneratorButton, &QPushButton::clicked, this, &MainWindow::onStopGeneratorClicked);
    connect(startAnalysisAllButton, &QPushButton::clicked, this, &MainWindow::onStartAnalysisAllClicked);
    connect(startAnalysisTagsButton, &QPushButton::clicked, this, &MainWindow::onStartAnalysisTagsClicked);
    connect(startAnalysisNonTagsButton, &QPushButton::clicked, this, &MainWindow::onStartAnalysisNonTagsClicked);
    connect(stopAnalysisButton, &QPushButton::clicked, this, &MainWindow::onStopAnalysisClicked);
    connect(loadTopEventsButton, &QPushButton::clicked, this, &MainWindow::onLoadTopEventsClicked);
    connect(loadIsolatedEventsButton, &QPushButton::clicked, this, &MainWindow::onLoadIsolatedEventsClicked);
}

void MainWindow::updateStatus() {
    QString statusText = QString("Generator: %1 | Analyzer: %2")
                            .arg(isGeneratorRunning ? "Running" : "Stopped")
                            .arg(isAnalyzerRunning ? "Running" : "Stopped");
    statusBar->showMessage(statusText);
}

void MainWindow::onStartGeneratorClicked() {
    if (!isGeneratorRunning && !generatorThread->isRunning()) {
        isGeneratorRunning = true;
        updateStatus();
        generatorThread->start();
        statusBar->showMessage("Generator started.", 2000);
    }
}

void MainWindow::onStopGeneratorClicked() {
    if (isGeneratorRunning) {
        writeStream->Close(); // Закрываем поток, чтобы прервать генератор и анализатор
        generatorThread->quit();
        generatorThread->wait();
        isGeneratorRunning = false;
        updateStatus();
        statusBar->showMessage("Generator stopped.", 2000);
    }
}

void MainWindow::onStartAnalysisAllClicked() {
    if (!isAnalyzerRunning && !analyzerThread->isRunning()) {
        isAnalyzerRunning = true;
        updateStatus();
        // Для запуска конкретного режима нужно вызвать метод worker'а
        // Лучше создать enum и передавать его в startAnalysis
        // Пока что просто запускаем поток, и в слоте вызываем нужный метод
        // Или используем invokeMethod
        QMetaObject::invokeMethod(analyzerWorker, &AnalyzerWorker::startAnalysisAll, Qt::QueuedConnection);
        analyzerThread->start();
        statusBar->showMessage("Analyzer (All) started.", 2000);
    }
}

void MainWindow::onStartAnalysisTagsClicked() {
    if (!isAnalyzerRunning && !analyzerThread->isRunning()) {
        isAnalyzerRunning = true;
        updateStatus();
        QMetaObject::invokeMethod(analyzerWorker, &AnalyzerWorker::startAnalysisTagsOnly, Qt::QueuedConnection);
        analyzerThread->start();
        statusBar->showMessage("Analyzer (Tags Only) started.", 2000);
    }
}

void MainWindow::onStartAnalysisNonTagsClicked() {
    if (!isAnalyzerRunning && !analyzerThread->isRunning()) {
        isAnalyzerRunning = true;
        updateStatus();
        QMetaObject::invokeMethod(analyzerWorker, &AnalyzerWorker::startAnalysisNonTagsOnly, Qt::QueuedConnection);
        analyzerThread->start();
        statusBar->showMessage("Analyzer (Non-Tags Only) started.", 2000);
    }
}

void MainWindow::onStopAnalysisClicked() {
    if (isAnalyzerRunning) {
        writeStream->Close(); // Закрываем поток, чтобы прервать анализатор
        // analyzerWorker->stopAnalysis(); // Вызовется после закрытия потока
        // Попробуем вызвать stopAnalysis через invokeMethod, чтобы он выполнялся в потоке worker'а
        QMetaObject::invokeMethod(analyzerWorker, &AnalyzerWorker::stopAnalysis, Qt::QueuedConnection);
        analyzerThread->quit(); // Запрашиваем выход потока (он завершится после выполнения stopAnalysis)
        analyzerThread->wait(); // Ждем завершения потока
        isAnalyzerRunning = false;
        updateStatus();
        statusBar->showMessage("Analyzer stopped.", 2000);
    }
}

void MainWindow::onLoadTopEventsClicked() {
    topEventsList->clear();
    topEventsList->addItem("Top Event 1");
    topEventsList->addItem("Top Event 2");
}

void MainWindow::onLoadIsolatedEventsClicked() {
    isolatedEventsList->clear();
    isolatedEventsList->addItem("Isolated Event 1");
    isolatedEventsList->addItem("Isolated Event 2");
}

void MainWindow::onGeneratorFinished() {
    isGeneratorRunning = false;
    updateStatus();
    statusBar->showMessage("Generator finished.", 2000);
}

void MainWindow::onGeneratorError(const QString& message) {
    statusBar->showMessage("Generator error: " + message, 5000);
    isGeneratorRunning = false;
    updateStatus();
}

void MainWindow::onAnalyzerFinished() {
    isAnalyzerRunning = false;
    updateStatus();
    statusBar->showMessage("Analyzer finished.", 2000);
}

void MainWindow::onAnalyzerError(const QString& message) {
    statusBar->showMessage("Analyzer error: " + message, 5000);
    isAnalyzerRunning = false;
    updateStatus();
}

void MainWindow::onGraphVertexClicked(const QString& eventId) {
    detailsTextEdit->setText(QString("Details for event: %1").arg(eventId));
}