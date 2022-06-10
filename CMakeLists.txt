cmake_minimum_required(VERSION 3.21)
project(TVGGraphCalc)

#Import Algo
file (GLOB_RECURSE Algo_SOURCES CONFIGURE_DEPENDS "Algo/*.cpp")
file (GLOB_RECURSE Algo_HEADERS CONFIGURE_DEPENDS "Algo/*.h")

set (Algo_INCLUDE_DIRS "")
foreach (_headerFile ${Algo_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND Algo_INCLUDE_DIRS ${_dir})
endforeach()
list (REMOVE_DUPLICATES Algo_INCLUDE_DIRS)

#Import Graph
file (GLOB_RECURSE Graph_SOURCES CONFIGURE_DEPENDS "Graph/*.cpp")
file (GLOB_RECURSE Graph_HEADERS CONFIGURE_DEPENDS "Graph/*.h")

set (Graph_INCLUDE_DIRS "")
foreach (_headerFile ${Graph_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND Graph_INCLUDE_DIRS ${_dir})
endforeach()
list (REMOVE_DUPLICATES Graph_INCLUDE_DIRS)

set(CMAKE_CXX_STANDARD 20)

add_executable(TVGGraphCalc main.cpp Lib/thread-pool-2.0.0/thread_pool.hpp ${Graph_SOURCES} ${Algo_SOURCES} Algo/Thread_safe_queue.cpp Algo/Thread_safe_queue.h LoadException.h LoadExeption.cpp Algo/postProcess.cpp Algo/postProcess.h)
