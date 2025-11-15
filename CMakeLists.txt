cmake_minimum_required(VERSION 3.14)
project(OS_Laboratory_Work_3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Основное приложение
add_executable(Lab3
    main.cpp
    marker.cpp
)

# Включаем GoogleTest через FetchContent
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.tar.gz
)

# Для Windows важно установить эту опцию
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Загружаем GoogleTest
FetchContent_MakeAvailable(googletest)

# Создаем тестовое приложение
add_executable(Lab3_Tests
    tests/test_utils.cpp
    tests/test_marker.cpp
    tests/test_main.cpp
    marker.cpp
)

# Подключаем заголовочные файлы
target_include_directories(Lab3_Tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Линкуем GoogleTest
target_link_libraries(Lab3_Tests PRIVATE
    GTest::gtest_main
)

# Опционально: добавляем тесты в CTest
include(GoogleTest)
gtest_discover_tests(Lab3_Tests)

# Для удобства в Visual Studio группируем тесты в отдельную папку
set_target_properties(Lab3_Tests PROPERTIES FOLDER "Tests")
set_target_properties(Lab3 PROPERTIES FOLDER "App")