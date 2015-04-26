cmake_minimum_required(VERSION 3.1)
project(Wet1_semifinal)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    commands.c
    commands.h
    jobs.c
    jobs.h
    signals.c
    signals.h
    smash.c)

add_executable(Wet1_semifinal ${SOURCE_FILES})