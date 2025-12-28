// UserInterface.cpp
#include "UserInterface.hpp"

UserInterface::UserInterface() : read_stream(write_stream), generator(write_stream), analyzer(read_stream) {}

void UserInterface::Run() {
    int choice;
    do {
        ShowMenu();
        std::cin >> choice;
        std::cin.ignore(); // Игнорируем символ новой строки после числа

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
                StopAnalyzer();
                break;
            case 5:
                DisplayResults();
                break;
            case 6:
                DisplayEventDetails();
                break;
            case 0:
                std::cout << "Stopping generator and analyzer..." << std::endl;
                StopAnalyzer(); // Сначала останавливаем анализатор
                StopGenerator(); // Потом останавливаем генератор
                break;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    } while (choice != 0);

    std::cout << "Exiting..." << std::endl;
}

void UserInterface::ShowMenu() {
    // Обновляем флаг, если анализатор завершился сам
    if (analyzer_running && !analyzer.IsRunning()) {
        analyzer_running = false;
        std::cout << "Analyzer finished automatically." << std::endl; // Опциональное сообщение
    }

    std::cout << "\n--- Event Stream Analyzer ---" << std::endl;
    std::cout << "Current Status:" << std::endl;
    std::cout << "  Generator: " << (generator_running ? "Running" : "Stopped") << std::endl;
    std::cout << "  Analyzer:  " << (analyzer_running ? "Running" : "Stopped") << std::endl;
    std::cout << "\nMenu:" << std::endl;
    std::cout << "1. Start Generator" << std::endl;
    std::cout << "2. Stop Generator" << std::endl;
    std::cout << "3. Start Analyzer" << std::endl;
    std::cout << "4. Stop Analyzer" << std::endl;
    std::cout << "5. Display Graph Results" << std::endl;
    std::cout << "6. Display Event Details" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "Enter your choice: ";
}

void UserInterface::StartGenerator() {
    if (!generator_running) {
        generator.Start();
        generator_running = true;
        std::cout << "Generator started." << std::endl;
    } else {
        std::cout << "Generator is already running." << std::endl;
    }
}

void UserInterface::StopGenerator() {
    if (generator_running) {
        generator.Stop();
        generator.Join(); // Ждем завершения потока
        write_stream.Close(); // Закрываем поток записи, чтобы уведомить читающий поток
        generator_running = false;
        std::cout << "Generator stopped and stream closed." << std::endl;
    } else {
        std::cout << "Generator is not running." << std::endl;
    }
}

void UserInterface::StartAnalyzer() {
    if (!analyzer_running) {
        analyzer.Start();
        analyzer_running = true;
        std::cout << "Analyzer started." << std::endl;
    } else {
        std::cout << "Analyzer is already running." << std::endl;
    }
}

void UserInterface::StopAnalyzer() {
    if (analyzer_running) {
        // Устанавливаем флаг остановки для анализатора
        // analyzer.Stop(); // Убираем, так как Stop больше не нужен в EventAnalyzer
        // Вместо этого, просто ждем, если он еще работает
        analyzer.Join(); // Ждем завершения потока
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
    // Проверяем, пуст ли граф, используя GetLength()
    auto vertices = analyzer.GetGraph().GetVertices();
    if (vertices.GetLength() == 0) { // Заменяем .empty() на .GetLength() == 0
        std::cout << "Graph is empty. No events to display." << std::endl;
        return;
    }

    std::cout << "Enter the Event ID to view details (or type 'list' to see all IDs): ";
    std::string input;
    std::cin >> input;

    if (input == "list") {
        std::cout << "Available Event IDs:" << std::endl;
        // Используем цикл с индексом вместо range-based for
        for (size_t i = 0; i < vertices.GetLength(); ++i) { // Используем GetLength()
            std::cout << "  - " << vertices.Get(i) << std::endl; // Используем Get(i)
        }
        std::cout << std::endl;
        // Снова запросим ID
        std::cout << "Enter the Event ID to view details: ";
        std::cin >> input;
    }

    // Вызов метода из графа для печати деталей
    analyzer.GetGraph().PrintEventDetails(input);
}
