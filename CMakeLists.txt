cmake_minimum_required(VERSION 3.26.4)

project(crypticizer)

add_executable(crypticizer src/crypticizer.cpp src/menu.cpp)
target_link_libraries(crypticizer -lncurses)

