cmake_minimum_required(VERSION 3.16)
project(software-challenge-2022)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O2 -march=broadwell")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")

if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE include-what-you-use)
endif (CMAKE_BUILD_TYPE EQUAL "DEBUG")

find_library(pugixmlpath NAMES libpugixml.a libpugixml.so)
add_library(pugixml STATIC IMPORTED)
set_target_properties(pugixml PROPERTIES IMPORTED_LOCATION ${pugixmlpath})

add_executable(main src/main.cpp src/alphabeta.cpp src/evaluation.cpp src/gamestate.cpp src/network.cpp src/parser.cpp src/transpositiontable.cpp)
target_link_libraries(main pugixml)
target_link_libraries(main pthread)

add_executable(perft utils/perft.cpp src/gamestate.cpp)

add_executable(elotest utils/elotest.cpp src/alphabeta.cpp src/evaluation.cpp src/gamestate.cpp src/transpositiontable.cpp)
target_link_libraries(elotest pthread)
set_target_properties(elotest PROPERTIES COMPILE_FLAGS -fno-access-control)

add_executable(crosstest utils/crosstest.cpp src/gamestate.cpp)
target_link_libraries(crosstest pthread)

add_executable(bench utils/bench.cpp src/alphabeta.cpp src/evaluation.cpp src/gamestate.cpp src/transpositiontable.cpp)
target_link_libraries(bench benchmark)
set_target_properties(bench PROPERTIES COMPILE_FLAGS -fno-access-control)
