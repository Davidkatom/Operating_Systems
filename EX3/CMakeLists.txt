cmake_minimum_required(VERSION 3.10)

# set the project name
project(EX3)

# add the executable
add_executable(EX3 bounded_buffer.c bounded_buffer.h main.c producer.c producer.h dispatcher.c dispatcher.h screenManager.c screenManager.h)

# specify the C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# link the pthread library
find_package(Threads REQUIRED)
target_link_libraries(EX3 Threads::Threads)
