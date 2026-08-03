#versione CMake
cmake_minimum_required(VERSION 3.20)

#nome progetto
project(progetto)

#versione C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wshadow -Werror=vla")

#eseguibile
add_executable(progetto progetto.cpp functions.cpp)

#aggiungo Eigen
target_include_directories(progetto PRIVATE /usr/include/eigen3)
