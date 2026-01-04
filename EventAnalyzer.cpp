#include "EventAnalyzer.hpp"
#include <iostream>

EventAnalyzer::EventAnalyzer(LazyReadStream& read_stream) : read_stream(read_stream) {}

void EventAnalyzer::Start() {
    if (!analyzer_thread.joinable()) {
        should_stop = false; 
        analyzer_thread = std::thread(&EventAnalyzer::AnalyzeLoop, this);
    }
}

void EventAnalyzer::StartAnalysisByTagsOnly() {
    if (!analyzer_thread.joinable()) {
        should_stop = false; 
        analyzer_thread = std::thread(&EventAnalyzer::AnalyzeLoopByTagsOnly, this);
    }
}

void EventAnalyzer::StartAnalysisByNonTagsOnly() {
    if (!analyzer_thread.joinable()) {
        should_stop = false; 
        analyzer_thread = std::thread(&EventAnalyzer::AnalyzeLoopByNonTagsOnly, this);
    }
}

void EventAnalyzer::Stop() { 
    should_stop = true; 
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
            graph.ProcessEventForSimilarity(event, tag_weight, message_weight, time_weight, user_id_weight, ip_weight, true, true, true, true, true);
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

void EventAnalyzer::AnalyzeLoopByTagsOnly() {
    while (!should_stop) {
        try {
            Event event = read_stream.Read();
            graph.ProcessEventForSimilarity(event, tag_weight, 0.0, 0.0, 0.0, 0.0, true, false, false, false, false);
        } catch (const EndOfStreamException& e) {
            std::cout << "Analyzer (Tags Only) caught EndOfStreamException. Stopping." << std::endl;
            break;
        } catch (const std::exception& e) {
            std::cerr << "Analyzer (Tags Only) error reading from stream: " << e.what() << std::endl;
            break;
        }

        if (should_stop) {
             std::cout << "Analyzer (Tags Only) stop requested. Stopping." << std::endl;
             break;
        }
    }
    std::cout << "EventAnalyzer (Tags Only) stopped." << std::endl;
}

void EventAnalyzer::AnalyzeLoopByNonTagsOnly() {
    while (!should_stop) {
        try {
            Event event = read_stream.Read();
            graph.ProcessEventForSimilarity(event, 0.0, message_weight, time_weight, user_id_weight, ip_weight, false, true, true, true, true);
        } catch (const EndOfStreamException& e) {
            std::cout << "Analyzer (Non-Tags Only) caught EndOfStreamException. Stopping." << std::endl;
            break;
        } catch (const std::exception& e) {
            std::cerr << "Analyzer (Non-Tags Only) error reading from stream: " << e.what() << std::endl;
            break;
        }

        if (should_stop) {
             std::cout << "Analyzer (Non-Tags Only) stop requested. Stopping." << std::endl;
             break;
        }
    }
    std::cout << "EventAnalyzer (Non-Tags Only) stopped." << std::endl;
}

const DependencyGraph& EventAnalyzer::GetGraph() const {
    return graph;
}