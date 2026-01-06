# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(LogFilesAnalyser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Путь к Qt
list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt")

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

# Явно перечисляем исходники
set(SOURCES
    main.cpp
    MainWindow.cpp
    GeneratorWorker.cpp
    AnalyzerWorker.cpp
    # Остальные ваши файлы
    UserInterface.cpp
    Event.cpp
    EventAnalyzer.cpp
    Dictionary.cpp
    EventGenerator.cpp
    LazyReadStream.cpp
    LazyWriteStream.cpp
    DependencyGraph.cpp
)

# Заголовочные файлы для MOC
set(HEADERS
    MainWindow.hpp
    GeneratorWorker.hpp
    AnalyzerWorker.hpp
    # Если у вас есть другие .hpp файлы с Q_OBJECT, добавьте их сюда
)

add_executable(lab5_app ${SOURCES} ${HEADERS})

target_link_libraries(lab5_app Qt6::Widgets Qt6::Core Qt6::Gui)
target_include_directories(lab5_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})