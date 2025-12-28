#include "LazyReadStream.hpp"
#include <chrono>

LazyReadStream::LazyReadStream(LazyWriteStream& source_stream) : source_stream(source_stream) {}

Event LazyReadStream::Read() {
    std::unique_lock<std::mutex> local_lock(local_buffer_mutex);
    while (local_buffer.empty()) {
        if (source_stream.IsClosed()) {
             if (local_buffer.empty()) {
                 throw EndOfStreamException();
             }
             break; 
        }
        std::unique_lock<std::mutex> source_lock(source_stream.buffer_mutex);
        source_stream.cv.wait_for(source_lock, std::chrono::milliseconds(100),
                                  [this] { return !source_stream.buffer.empty() || source_stream.closed; });

        while (!source_stream.buffer.empty()) {
            local_buffer.push(std::move(source_stream.buffer.front()));
            source_stream.buffer.pop();
        }
        source_lock.unlock();
        if (source_stream.IsClosed() && local_buffer.empty()) {
            throw EndOfStreamException();
        }
    }

    if (local_buffer.empty()) {
        throw EndOfStreamException();
    }

    Event event = std::move(local_buffer.front());
    local_buffer.pop();
    return event;
}

bool LazyReadStream::IsEndOfStream() const {
    std::lock_guard<std::mutex> lock(local_buffer_mutex);
    return local_buffer.empty() && source_stream.IsClosed();
}

bool LazyReadStream::HasData() const {
    std::lock_guard<std::mutex> lock(local_buffer_mutex);
    return !local_buffer.empty() || !source_stream.IsClosed();
}