cmake_minimum_required(VERSION 3.21)
project(TVGGraphCalc)

#Profile
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")


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
find_package (Threads)
add_executable(TVGGraphCalc main.cpp Lib/thread-pool-2.0.0/thread_pool.hpp ${Graph_SOURCES} ${Algo_SOURCES} Algo/DataStruct/Thread_safe_queue.cpp Algo/DataStruct/Thread_safe_queue.h LoadException.h LoadExeption.cpp Algo/postProcess.cpp Algo/postProcess.h Algo/DataStruct/bestRouteContainer.cpp Algo/DataStruct/GraphDataStruct.h Algo/DataStruct/DataNode.cpp Algo/DataStruct/DataNode.h Utility.h)
target_link_libraries (TVGGraphCalc ${CMAKE_THREAD_LIBS_INIT})
