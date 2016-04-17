
find_package(Threads REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/common)
set(SOURCE_FILES main.c)
set(LIBS ${LIBS} Threads::Threads m)
add_executable(producer ${SOURCE_FILES})
target_link_libraries(producer common ${LIBS})