# set the project name
project(EX3)

# add the executable
add_executable(EX3 bounded_buffer.c bounded_buffer.h main.c producer.c producer.h dispatcher.c dispatcher.h screenManager.c screenManager.h)

# specify the C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

