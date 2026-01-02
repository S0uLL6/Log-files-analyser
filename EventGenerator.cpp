#include "EventGenerator.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

std::string GenerateRandomWord(std::mt19937& gen) {
    static const std::string vowels = "aeiouy";
    static const std::string consonants = "bcdfghjklmnpqrstvwxz";
    std::uniform_int_distribution<> syllable_count_dist(1, 3);
    std::uniform_int_distribution<> vowel_dist(0, vowels.length() - 1);
    std::uniform_int_distribution<> consonant_dist(0, consonants.length() - 1);

    int syllables = syllable_count_dist(gen);
    std::string word;

    for (int s = 0; s < syllables; ++s) {
        if (s == 0 || gen() % 100 < 70) {
            word += consonants[consonant_dist(gen)];
        }
        word += vowels[vowel_dist(gen)];
        if (gen() % 100 < 50) {
            word += consonants[consonant_dist(gen)];
        }
    }

    if (gen() % 100 < 30) {
        static const std::string suffixes[] = {"ing", "ed", "er", "est", "ly", "tion", "ness", "ment", "ful", "less"};
        word += suffixes[gen() % (sizeof(suffixes) / sizeof(suffixes[0]))];
    }

    if (gen() % 100 < 10) {
        static const std::string prefixes[] = {"un", "re", "pre", "over", "under", "out", "up", "down", "in", "out"};
        word = prefixes[gen() % (sizeof(prefixes) / sizeof(prefixes[0]))] + word;
    }

    return word;
}

EventGenerator::EventGenerator(LazyWriteStream& write_stream)
    : write_stream(write_stream),
      gen(rd()),
      id_dist(1, 1000000),
      tag_count_dist(1, 4),
      attr_dist(0, 100),
      word_count_dist(5, 15)
{
    static const std::string event_type_data[] = {
        "INFO", "WARNING", "ERROR", "DEBUG", "USER_ACTION", "PERFORMANCE_ISSUE", "SYSTEM_START", "SYSTEM_STOP"
    };
    static const std::string possible_tag_data[] = {
        "security", "performance", "user", "system", "database", "network", "io", "cache", "auth", "config", "error_handling"
    };
    static const std::string possible_user_id_data[] = {
        "user_001", "user_002", "admin_01", "guest_123", "service_a", "service_b", "user_999", "user_555"
    };
    static const std::string possible_ip_data[] = {
        "192.168.1.10", "10.0.0.5", "172.16.0.100", "192.168.1.200", "10.0.0.10", "172.16.0.50", "192.168.1.150", "10.0.0.15"
    };

    event_types = ArraySequence<std::string>(event_type_data, sizeof(event_type_data) / sizeof(event_type_data[0]));
    possible_tags = ArraySequence<std::string>(possible_tag_data, sizeof(possible_tag_data) / sizeof(possible_tag_data[0]));
    possible_user_ids = ArraySequence<std::string>(possible_user_id_data, sizeof(possible_user_id_data) / sizeof(possible_user_id_data[0]));
    possible_ips = ArraySequence<std::string>(possible_ip_data, sizeof(possible_ip_data) / sizeof(possible_ip_data[0]));

    InitializeDistributions();
}

void EventGenerator::InitializeDistributions() {
    if (event_types.GetLength() > 0) {
        type_dist = std::uniform_int_distribution<>(0, static_cast<int>(event_types.GetLength()) - 1);
    }
    if (possible_tags.GetLength() > 0) {
        tag_dist = std::uniform_int_distribution<>(0, static_cast<int>(possible_tags.GetLength()) - 1);
    }
    if (possible_user_ids.GetLength() > 0) {
        user_id_dist = std::uniform_int_distribution<>(0, static_cast<int>(possible_user_ids.GetLength()) - 1);
    }
    if (possible_ips.GetLength() > 0) {
        ip_dist = std::uniform_int_distribution<>(0, static_cast<int>(possible_ips.GetLength()) - 1);
    }
}

void EventGenerator::Start() {
    if (!generator_thread.joinable()) {
        should_stop = false;
        generator_thread = std::thread(&EventGenerator::GenerateLoop, this);
    }
}

void EventGenerator::Stop() {
    should_stop = true;
}

void EventGenerator::Join() {
    if (generator_thread.joinable()) {
        generator_thread.join();
    }
}

void EventGenerator::GenerateLoop() {
    if (event_types.GetLength() == 0 || possible_tags.GetLength() == 0 || possible_user_ids.GetLength() == 0 || possible_ips.GetLength() == 0) {
        std::cerr << "Error: EventGenerator has empty types, tags, user IDs, or IPs." << std::endl;
        return;
    }

    while (!should_stop) {
        std::string id = "EVT_" + std::to_string(id_dist(gen));
        std::string type = event_types.Get(type_dist(gen));

        int num_words = word_count_dist(gen);
        std::ostringstream msg_stream;
        for (int i = 0; i < num_words; ++i) {
            if (i > 0) msg_stream << " ";
            msg_stream << GenerateRandomWord(gen);
        }
        std::string message = msg_stream.str();

        int num_tags = tag_count_dist(gen);
        if (num_tags < 1) {
            num_tags = 1;
        }

        MutableArraySequence<std::string> unique_tags_temp;
        int attempts = 0;
        int added_count = 0;

        while (added_count < num_tags && attempts < num_tags * 5) {
            std::string new_tag = possible_tags.Get(tag_dist(gen));

            bool is_unique = true;
            for (size_t j = 0; j < unique_tags_temp.GetLength(); ++j) {
                if (unique_tags_temp.Get(j) == new_tag) {
                    is_unique = false;
                    break;
                }
            }

            if (is_unique) {
                unique_tags_temp.Append(new_tag);
                added_count++;
            }
            attempts++;
        }

        ArraySequence<std::string> final_tags(unique_tags_temp.GetLength());
        for (size_t idx = 0; idx < unique_tags_temp.GetLength(); ++idx) {
             final_tags.SetAt(idx, unique_tags_temp.Get(idx));
        }

        std::string user_id = possible_user_ids.Get(user_id_dist(gen));
        std::string ip_address = possible_ips.Get(ip_dist(gen));

        Event event(id, type, message, final_tags, user_id, ip_address, std::chrono::steady_clock::now()); // Передаем новые поля

        try {
            write_stream.Write(event);
        } catch (const std::exception& e) {
            std::cerr << "Generator error writing to stream: " << e.what() << std::endl;
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100 + (attr_dist(gen) % 200)));
    }
    std::cout << "EventGenerator stopped." << std::endl;
}