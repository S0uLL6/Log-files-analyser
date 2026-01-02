#include "Event.hpp"
#include <sstream>

Event::Event()
    : id(""),
      type(""),
      message(""),
      tags(ArraySequence<std::string>(1)),
      user_id(""), 
      ip_address(""), 
      timestamp(std::chrono::steady_clock::now()) {}

Event::Event(const std::string& id, const std::string& type, const std::string& message,
             const ArraySequence<std::string>& tags,
             const std::string& user_id, const std::string& ip_address, 
             const std::chrono::steady_clock::time_point& timestamp)
    : id(id),
      type(type),
      message(message),
      tags(tags),
      user_id(user_id), 
      ip_address(ip_address), 
      timestamp(timestamp) {}

std::string Event::GetId() const { return id; }
std::string Event::GetType() const { return type; }
std::string Event::GetMessage() const { return message; }
const ArraySequence<std::string>& Event::GetTags() const { return tags; }
std::string Event::GetUserId() const { return user_id; }
std::string Event::GetIpAddress() const { return ip_address; } 
std::chrono::steady_clock::time_point Event::GetTimestamp() const { return timestamp; }

std::string Event::ToString() const {
    std::ostringstream oss;
    oss << "[ID: " << id << ", Type: " << type << ", Tags: ";
    for (size_t i = 0; i < tags.GetLength(); ++i) {
        if (i > 0) oss << ", ";
        oss << tags.Get(i);
    }
    oss << ", User ID: " << user_id << ", IP: " << ip_address 
        << ", Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.time_since_epoch()).count() << "]";
    return oss.str();
}