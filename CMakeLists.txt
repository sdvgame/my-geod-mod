cmake_minimum_required(VERSION 3.25)

set(PROJECT_NAME TrailTracker)
project(${PROJECT_NAME} VERSION 1.0.0)

add_library(${PROJECT_NAME} SHARED
    src/main.cpp
    src/TrailTracker.cpp
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

find_package(Geode REQUIRED)

setup_geode_mod(${PROJECT_NAME})