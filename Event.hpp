// Event.h
#ifndef EVENT_H
#define EVENT_H

#include "Sequence.hpp" 
#include <string>
#include <chrono>

class Event {
public:
    Event();
    Event(const std::string& id, const std::string& type, const std::string& message,
          const ArraySequence<std::string>& tags, const std::chrono::steady_clock::time_point& timestamp);

    std::string GetId() const;
    std::string GetType() const;
    std::string GetMessage() const;
    const ArraySequence<std::string>& GetTags() const; 
    std::chrono::steady_clock::time_point GetTimestamp() const;

    std::string ToString() const;

private:
    std::string id;
    std::string type;
    std::string message;
    ArraySequence<std::string> tags; 
    std::chrono::steady_clock::time_point timestamp;
};

#endif // EVENT_H