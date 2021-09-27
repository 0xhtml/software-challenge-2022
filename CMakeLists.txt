cmake_minimum_required(VERSION 3.16)
project(software-challenge-2022)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O3")

add_executable(main src/main.cpp src/alphabeta.cpp src/evaluation.cpp src/gamestate.cpp src/network.cpp src/parser.cpp src/transpositiontable.cpp)
target_link_libraries(main pugixml)
target_link_libraries(main pthread)

add_executable(perft utils/perft.cpp src/gamestate.cpp)

add_executable(test utils/test.cpp src/alphabeta.cpp src/evaluation.cpp src/gamestate.cpp src/transpositiontable.cpp)
target_link_libraries(test pthread)
