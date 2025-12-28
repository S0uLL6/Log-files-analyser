// main.cpp
#include "UserInterface.hpp"

int main() {
    UserInterface ui;
    ui.Run();
    return 0;
}

// g++ main.cpp UserInterface.cpp Event.cpp LazyWriteStream.cpp LazyReadStream.cpp EventGenerator.cpp DependencyGraph.cpp EventAnalyzer.cpp -o lab5_app -std=c++17 -pthread