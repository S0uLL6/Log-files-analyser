// EventAnalyzer.h
#ifndef EVENT_ANALYZER_H
#define EVENT_ANALYZER_H

#include "Event.hpp"
#include "LazyReadStream.hpp"
#include "DependencyGraph.hpp"
#include <thread>
#include <atomic>

class EventAnalyzer {
public:
    EventAnalyzer(LazyReadStream& read_stream);

    void Start();
    void Join();
    const DependencyGraph& GetGraph() const;
    bool IsRunning() const;

private:
    void AnalyzeLoop();

    LazyReadStream& read_stream;
    DependencyGraph graph;
    std::thread analyzer_thread;
    std::atomic<bool> should_stop{false};

    // --- Новые параметры ---
    double tag_weight = 1.0;
    double message_weight = 1.0;
    // -----------------------
};

#endif // EVENT_ANALYZER_H