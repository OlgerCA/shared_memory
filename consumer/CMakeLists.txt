
include_directories(${CMAKE_SOURCE_DIR}/common)
set(SOURCE_FILES main.c)
add_executable(consumer ${SOURCE_FILES})
target_link_libraries(consumer common)