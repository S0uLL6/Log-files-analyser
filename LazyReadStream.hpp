// LazyReadStream.h
#ifndef LAZY_READ_STREAM_H
#define LAZY_READ_STREAM_H

#include "Event.hpp"
#include "LazyWriteStream.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>

class LazyReadStream {
public:
    LazyReadStream(LazyWriteStream& source_stream);
    Event Read();
    bool IsEndOfStream() const;
    bool HasData() const;

private:
    LazyWriteStream& source_stream;
    std::queue<Event> local_buffer;
    mutable std::mutex local_buffer_mutex;
};

#endif // LAZY_READ_STREAM_H