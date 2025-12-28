// LazyWriteStream.cpp
#include "LazyWriteStream.hpp"

LazyWriteStream::LazyWriteStream() : closed(false) {}

void LazyWriteStream::Write(const Event& event) {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    if (closed) {
        throw std::runtime_error("Cannot write to a closed stream.");
    }
    buffer.push(event);
    cv.notify_one(); // Уведомляем поток чтения о новом событии
}

void LazyWriteStream::Close() {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    closed = true;
    cv.notify_all(); // Уведомляем все ожидающие потоки
}

bool LazyWriteStream::IsClosed() const {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    return closed;
}

size_t LazyWriteStream::GetBufferSize() const {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    return buffer.size();
}