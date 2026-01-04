#ifndef EVENT_GENERATOR_H
#define EVENT_GENERATOR_H

#include "Event.hpp"
#include "LazyWriteStream.hpp"
#include "Sequence.hpp"
#include "Dictionary.hpp" 
#include <random>
#include <thread>
#include <chrono>
#include <atomic>

class EventGenerator {
public:
    EventGenerator(LazyWriteStream& write_stream, const std::string& dictionary_file); 
    void Start();
    void Stop();
    void Join();

private:
    void GenerateLoop();
    void InitializeDistributions();

    LazyWriteStream& write_stream;
    std::thread generator_thread;
    std::atomic<bool> should_stop{false};

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> id_dist;
    std::uniform_int_distribution<> type_dist;
    std::uniform_int_distribution<> tag_count_dist;
    std::uniform_int_distribution<> tag_dist;
    std::uniform_int_distribution<> user_id_dist;
    std::uniform_int_distribution<> ip_dist;
    std::uniform_int_distribution<> attr_dist;

    ArraySequence<std::string> event_types;
    ArraySequence<std::string> possible_tags;
    ArraySequence<std::string> possible_user_ids;
    ArraySequence<std::string> possible_ips;

    Dictionary dictionary;
};

#endif // EVENT_GENERATOR_H