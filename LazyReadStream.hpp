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
    // Конструктор, принимает ссылку на поток записи
    LazyReadStream(LazyWriteStream& source_stream);

    // Читает следующее событие из потока. Может блокировать.
    // Выбрасывает EndOfStreamException, если поток закрыт и буфер пуст.
    Event Read();

    // Проверяет, достигнут ли конец потока (закрыт и буфер пуст).
    bool IsEndOfStream() const;

    // Проверяет, есть ли данные, доступные для чтения прямо сейчас, или есть вероятность, что они появятся.
    // true - данные есть или могут появиться (поток не закрыт), false - поток закрыт и буфер пуст.
    bool HasData() const;

private:
    LazyWriteStream& source_stream;
    std::queue<Event> local_buffer;
    mutable std::mutex local_buffer_mutex;
};

#endif // LAZY_READ_STREAM_H