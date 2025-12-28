#ifndef LAZY_WRITE_STREAM_H
#define LAZY_WRITE_STREAM_H

#include "Event.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>

class EndOfStreamException : public std::exception {
public:
    const char* what() const noexcept override {
        return "End of stream reached.";
    }
};

class LazyReadStream; 

class LazyWriteStream {
    friend class LazyReadStream; 
public:
    LazyWriteStream();

    void Write(const Event& event);

    void Close();

    bool IsClosed() const;

    size_t GetBufferSize() const;

private:
    std::queue<Event> buffer;
    mutable std::mutex buffer_mutex;
    std::condition_variable cv;
    bool closed;
};

#endif // LAZY_WRITE_STREAM_H