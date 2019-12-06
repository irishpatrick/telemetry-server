cmake_minimum_required(VERSION 3.0)

project(telemserver)

file(GLOB sources *.c *.h)
set(FLAGS -Wall -g -pthread)

add_executable(${PROJECT_NAME} ${sources})

target_compile_options(${PROJECT_NAME} PRIVATE ${FLAGS})
target_link_libraries(${PROJECT_NAME} PRIVATE -pthread)
