#include "UserInterface.hpp"
#include <iostream>

UserInterface::UserInterface() : read_stream(write_stream), analyzer(read_stream) {
    generator = nullptr;
}

UserInterface::~UserInterface() {
    if (generator_running && generator != nullptr) {
        StopGenerator();
    }
    if (analyzer_running) {
        StopAnalyzer();
    }
    delete generator;
}

void UserInterface::Run() {
    int choice;
    do {
        ShowMenu();
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
            case 1:
                StartGenerator();
                break;
            case 2:
                StopGenerator();
                break;
            case 3:
                StartAnalyzer();
                break;
            case 4:
                StartAnalyzerByTagsOnly(); 
                break;
            case 5:
                StartAnalyzerByNonTagsOnly(); 
                break;
            case 6:
                StopAnalyzer();
                break;
            case 7:
                DisplayResults();
                break;
            case 8:
                DisplayEventDetails();
                break;
            case 9:
                DisplayTopConnectedEvents();
                break;
            case 10:
                DisplayIsolatedEvents();
                break;
            case 0:
                std::cout << "Stopping generator and analyzer..." << std::endl;
                StopAnalyzer();
                StopGenerator();
                break;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    } while (choice != 0);

    std::cout << "Exiting..." << std::endl;
}

void UserInterface::ShowMenu() {
    if (analyzer_running && !analyzer.IsRunning()) {
        analyzer_running = false;
        std::cout << "Analyzer finished automatically." << std::endl;
    }

    std::cout << "\n--- Event Stream Analyzer ---" << std::endl;
    std::cout << "Current Status:" << std::endl;
    std::cout << "  Generator: " << (generator_running ? "Running" : "Stopped") << std::endl;
    std::cout << "  Analyzer:  " << (analyzer_running ? "Running" : "Stopped") << std::endl;
    std::cout << "\nMenu:" << std::endl;
    std::cout << "1. Start Generator" << std::endl;
    std::cout << "2. Stop Generator" << std::endl;
    std::cout << "3. Start Analyzer (All)" << std::endl; 
    std::cout << "4. Start Analyzer (Tags Only)" << std::endl; 
    std::cout << "5. Start Analyzer (Non-Tags Only)" << std::endl; 
    std::cout << "6. Stop Analyzer" << std::endl;
    std::cout << "7. Display Graph Results" << std::endl;
    std::cout << "8. Display Event Details" << std::endl;
    std::cout << "9. Display Top Connected Events" << std::endl;
    std::cout << "10. Display Isolated Events" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "Enter your choice: ";
}

void UserInterface::StartGenerator() {
    if (!generator_running && generator == nullptr) {
        try {
            generator = new EventGenerator(write_stream, "dictionary.json");
            generator->Start();
            generator_running = true;
            std::cout << "Generator started." << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Failed to start generator: " << e.what() << std::endl;
            delete generator;
            generator = nullptr;
            generator_running = false;
        }
    } else if (generator_running) {
        std::cout << "Generator is already running." << std::endl;
    } else {
        std::cout << "Generator already exists but is not running." << std::endl;
    }
}

void UserInterface::StopGenerator() {
    if (generator_running && generator != nullptr) {
        generator->Stop();
        generator->Join();
        write_stream.Close();
        delete generator;
        generator = nullptr;
        generator_running = false;
        std::cout << "Generator stopped and stream closed." << std::endl;
    } else {
        std::cout << "Generator is not running." << std::endl;
    }
}

void UserInterface::StartAnalyzer() {
    if (analyzer_running) {
        std::cout << "Stopping current analyzer mode..." << std::endl;
        StopAnalyzer(); // Останавливаем предыдущий режим
    }
    analyzer.Start();
    analyzer_running = true;
    std::cout << "Analyzer (All) started." << std::endl;
}

void UserInterface::StartAnalyzerByTagsOnly() {
    if (analyzer_running) {
        std::cout << "Stopping current analyzer mode..." << std::endl;
        StopAnalyzer();
    }
    analyzer.StartAnalysisByTagsOnly();
    analyzer_running = true;
    std::cout << "Analyzer (Tags Only) started." << std::endl;
}

void UserInterface::StartAnalyzerByNonTagsOnly() {
    if (analyzer_running) {
        std::cout << "Stopping current analyzer mode..." << std::endl;
        StopAnalyzer();
    }
    analyzer.StartAnalysisByNonTagsOnly();
    analyzer_running = true;
    std::cout << "Analyzer (Non-Tags Only) started." << std::endl;
}

void UserInterface::StopAnalyzer() {
    if (analyzer_running) {
        analyzer.Stop(); 
        analyzer.Join(); 
        analyzer_running = false;
        std::cout << "Analyzer stopped." << std::endl;
    } else {
        std::cout << "Analyzer is not running." << std::endl;
    }
}


void UserInterface::DisplayResults() {
    if (analyzer_running) {
        std::cout << "Analyzer is still running. Results might be incomplete." << std::endl;
    }
    analyzer.GetGraph().PrintGraph();
}

void UserInterface::DisplayEventDetails() {
    auto vertices = analyzer.GetGraph().GetVertices();
    if (vertices.GetLength() == 0) {
        std::cout << "Graph is empty. No events to display." << std::endl;
        return;
    }

    std::cout << "Enter the Event ID to view details (or type 'list' to see all IDs): ";
    std::string input;
    std::cin >> input;

    if (input == "list") {
        std::cout << "Available Event IDs:" << std::endl;
        for (size_t i = 0; i < vertices.GetLength(); ++i) {
            std::cout << "  - " << vertices.Get(i) << std::endl;
        }
        std::cout << std::endl;
        std::cout << "Enter the Event ID to view details: ";
        std::cin >> input;
    }

    analyzer.GetGraph().PrintEventDetails(input);
}

void UserInterface::DisplayTopConnectedEvents() {
    analyzer.GetGraph().PrintTopConnectedEvents();
}

void UserInterface::DisplayIsolatedEvents() {
    analyzer.GetGraph().PrintIsolatedEvents();
}