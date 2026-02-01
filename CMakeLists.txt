cmake_minimum_required(VERSION 4.1)
project(EchoEngine)

set(CMAKE_CXX_STANDARD 20)

add_executable(EchoEngine src/MainActivity.cpp
        src/core/Scanner.cpp
        src/model/FileRecord.h
        src/model/Snapshot.h
        src/core/Hash.cpp
        src/core/RuleEngine.h
        src/storage/SnapshotStorage.cpp
        src/core/Integrity.cpp
        src/model/FileInfo.h
)
