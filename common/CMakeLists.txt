
set(SOURCE_FILES Message.c Message.h SharedBuffer.c SharedBuffer.h Semaphore.c Semaphore.h)
add_library(common SHARED ${SOURCE_FILES})