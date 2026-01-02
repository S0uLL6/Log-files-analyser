#include "EventAnalyzer.hpp"
#include <iostream>

EventAnalyzer::EventAnalyzer(LazyReadStream& read_stream) : read_stream(read_stream) {}

void EventAnalyzer::Start() {
    if (!analyzer_thread.joinable()) {
        should_stop = false;
        analyzer_thread = std::thread(&EventAnalyzer::AnalyzeLoop, this);
    }
}

void EventAnalyzer::Join() {
    if (analyzer_thread.joinable()) {
        analyzer_thread.join();
    }
}

bool EventAnalyzer::IsRunning() const {
    return analyzer_thread.joinable();
}

void EventAnalyzer::AnalyzeLoop() {
    while (!should_stop) {
        try {
            Event event = read_stream.Read();
            graph.ProcessEventForSimilarity(event, tag_weight, message_weight, time_weight, user_id_weight, ip_weight); 
        } catch (const EndOfStreamException& e) {
            std::cout << "Analyzer caught EndOfStreamException. Stopping." << std::endl;
            break;
        } catch (const std::exception& e) {
            std::cerr << "Analyzer error reading from stream: " << e.what() << std::endl;
            break;
        }

        if (should_stop) {
             std::cout << "Analyzer stop requested. Stopping." << std::endl;
             break;
        }
    }
    std::cout << "EventAnalyzer stopped." << std::endl;
}

const DependencyGraph& EventAnalyzer::GetGraph() const {
    return graph;
}