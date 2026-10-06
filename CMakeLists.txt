 cmake_minimum_required(VERSION 3.25)

set(PROJECT_NAME TrailTracker)
project(${PROJECT_NAME} VERSION 1.0.0)

add_library(${PROJECT_NAME} SHARED
    src/main.cpp
    src/TrailTracker.cpp
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

# Подключаем SDK Geode напрямую, используя путь из переменной окружения
# Эту переменную GitHub Actions устанавливает автоматически
if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# Теперь, когда SDK подключён, функция setup_geode_mod доступна
setup_geode_mod(${PROJECT_NAME})