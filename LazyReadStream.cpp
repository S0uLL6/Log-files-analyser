// LazyReadStream.cpp
#include "LazyReadStream.hpp"
#include <chrono>

LazyReadStream::LazyReadStream(LazyWriteStream& source_stream) : source_stream(source_stream) {}

Event LazyReadStream::Read() {
    std::unique_lock<std::mutex> local_lock(local_buffer_mutex);

    // Цикл нужен, чтобы обработать "spurious wakeups" condition_variable
    // и чтобы проверять IsClosed() при каждом захвате мьютекса.
    while (local_buffer.empty()) {
        // Проверяем, закрыт ли поток *до* ожидания.
        if (source_stream.IsClosed()) {
             // Если локальный буфер пуст и источник закрыт, это конец потока.
             if (local_buffer.empty()) {
                 throw EndOfStreamException();
             }
             // Если буфер не пуст, но источник закрыт, мы читаем оставшиеся данные.
             // Цикл while выйдет, так как !local_buffer.empty().
             break; // Выходим из while, чтобы прочитать из local_buffer
        }

        // Если локальный буфер пуст, но источник не закрыт, ожидаем данные от источника
        // Используем wait_for с коротким таймаутом, чтобы периодически проверять IsClosed()
        std::unique_lock<std::mutex> source_lock(source_stream.buffer_mutex);
        // Ждем до 100мс или пока появятся данные/поток закроется
        source_stream.cv.wait_for(source_lock, std::chrono::milliseconds(100),
                                  [this] { return !source_stream.buffer.empty() || source_stream.closed; });

        // Перемещаем данные из источника в локальный буфер под локом источника
        while (!source_stream.buffer.empty()) {
            local_buffer.push(std::move(source_stream.buffer.front()));
            source_stream.buffer.pop();
        }
        source_lock.unlock(); // Отпускаем лок на источнике

        // Проверяем IsClosed() снова после освобождения лока источника
        // и перемещения данных. Если поток закрылся, и локальный буфер пуст,
        // значит, данных больше не будет.
        if (source_stream.IsClosed() && local_buffer.empty()) {
            throw EndOfStreamException();
        }
        // Если данные были перемещены, цикл while прервется, и мы пойдем к return
    }

    // Теперь локальный буфер должен быть не пуст
    if (local_buffer.empty()) {
        // Это состояние не должно быть достигнуто при правильной логике выше, но на всякий случай
        throw EndOfStreamException();
    }

    Event event = std::move(local_buffer.front());
    local_buffer.pop();
    // local_lock выходит из области видимости, освобождая local_buffer_mutex
    return event;
}

bool LazyReadStream::IsEndOfStream() const {
    std::lock_guard<std::mutex> lock(local_buffer_mutex);
    // Поток закончен, если локальный буфер пуст И источник закрыт
    return local_buffer.empty() && source_stream.IsClosed();
}

bool LazyReadStream::HasData() const {
    std::lock_guard<std::mutex> lock(local_buffer_mutex);
    // Есть данные, если локальный буфер не пуст, или источник не закрыт (и значит, могут появиться данные)
    return !local_buffer.empty() || !source_stream.IsClosed();
}