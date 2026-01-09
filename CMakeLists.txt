cmake_minimum_required(VERSION 4.1)
project(EchoEngine)

set(CMAKE_CXX_STANDARD 20)

add_executable(EchoEngine src/main.cpp
        src/modules/KEYLOGGER/keylogger.cpp
        include/EchoEngine/modules/keylogger/keylogger.h
        src/modules/KEYLOGGER/main_keylogger.cpp
        include/EchoEngine/modules/keylogger/main_keylogger.h
        src/utils/string_utils.cpp
        include/EchoEngine/utils/string_utils.h
        src/modules/openlink/openlink.cpp
        include/EchoEngine/modules/openlink/openlink.h
)
