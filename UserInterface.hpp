#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "EventGenerator.hpp"
#include "EventAnalyzer.hpp"
#include "LazyWriteStream.hpp"
#include "LazyReadStream.hpp"
#include <iostream>
#include <string>

class UserInterface {
public:
    UserInterface();
    ~UserInterface();
    void Run();

private:
    void ShowMenu();
    void StartGenerator();
    void StopGenerator();
    void StartAnalyzer();
    void StartAnalyzerByTagsOnly(); 
    void StartAnalyzerByNonTagsOnly(); 
    void StopAnalyzer();
    void DisplayResults();
    void DisplayEventDetails();
    void DisplayTopConnectedEvents();
    void DisplayIsolatedEvents();

    LazyWriteStream write_stream;
    LazyReadStream read_stream;
    EventGenerator* generator;
    EventAnalyzer analyzer; 

    bool generator_running = false;
    bool analyzer_running = false; 
};

#endif // USER_INTERFACE_H