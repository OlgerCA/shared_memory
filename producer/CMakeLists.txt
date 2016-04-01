
include_directories(${CMAKE_SOURCE_DIR}/common)
set(SOURCE_FILES main.c)
add_executable(producer ${SOURCE_FILES})
target_link_libraries(producer common)