cmake_minimum_required(VERSION 4.1)
project(EchoEngine)

set(CMAKE_CXX_STANDARD 20)

add_executable(EchoEngine scr/main.cpp
        scr/MainFunctional/KEYLOGGER/keylogger.cpp
        scr/MainFunctional/KEYLOGGER/keylogger.h
        scr/MainFunctional/KEYLOGGER/main_keylogger.cpp
        scr/MainFunctional/KEYLOGGER/main_keylogger.h
)
