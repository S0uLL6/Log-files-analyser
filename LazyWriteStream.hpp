// LazyWriteStream.h
#ifndef LAZY_WRITE_STREAM_H
#define LAZY_WRITE_STREAM_H

#include "Event.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>

// Исключение для конца потока
class EndOfStreamException : public std::exception {
public:
    const char* what() const noexcept override {
        return "End of stream reached.";
    }
};

class LazyReadStream; // Предварительное объявление

class LazyWriteStream {
    friend class LazyReadStream; // Позволяем LazyReadStream получать прямой доступ к внутренностям
public:
    // Конструктор
    LazyWriteStream();

    // Записывает событие в поток
    void Write(const Event& event);

    // Закрывает поток
    void Close();

    // Проверяет, закрыт ли поток
    bool IsClosed() const;

    // Получает размер буфера (для отладки)
    size_t GetBufferSize() const;

private:
    std::queue<Event> buffer;
    mutable std::mutex buffer_mutex;
    std::condition_variable cv;
    bool closed;
};

#endif // LAZY_WRITE_STREAM_H