// UserInterface.h
#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "EventGenerator.hpp"
#include "EventAnalyzer.hpp"
#include "LazyWriteStream.hpp"
#include "LazyReadStream.hpp"
#include <iostream>

class UserInterface {
public:
    UserInterface();
    void Run();

private:
    void ShowMenu();
    void StartGenerator();
    void StopGenerator();
    void StartAnalyzer();
    void StopAnalyzer(); // Должен быть реализован
    void DisplayResults();
    void DisplayEventDetails(); // Новый метод

    LazyWriteStream write_stream;
    LazyReadStream read_stream;
    EventGenerator generator;
    EventAnalyzer analyzer;

    bool generator_running = false;
    bool analyzer_running = false;
};

#endif // USER_INTERFACE_H