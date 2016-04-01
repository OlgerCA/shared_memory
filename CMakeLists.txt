cmake_minimum_required(VERSION 3.3)
project(shared_memory)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

add_subdirectory(common)
add_subdirectory(consumer)
add_subdirectory(finalizer)
add_subdirectory(initializer)
add_subdirectory(producer)
