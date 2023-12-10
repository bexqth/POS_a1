cmake_minimum_required(VERSION 3.18)
project(uloha1 C)

set(CMAKE_C_STANDARD 11)

add_executable(uloha1 main.c)
target_link_libraries(uloha1 pthread)
