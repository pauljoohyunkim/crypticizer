cmake_minimum_required(VERSION 3.26.4)

project(crypticizer)

add_executable(crypticizer src/crypticizer.cpp src/menu.cpp src/session.cpp src/cryptor.cpp)
target_link_libraries(crypticizer -lncurses)
target_compile_features(crypticizer PRIVATE cxx_std_17)

