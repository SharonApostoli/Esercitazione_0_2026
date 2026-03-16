#specifichiamo la versione di CMake da usare
cmake_minimum_required(VERSION 3.20)

#specifichiamo il nome del progetto
project(ese1)

#versione di C++ da utilizzare
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#incrementiamo il livello di warning
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wshadow")

#dichiariamo l'eseguibile
add_executable(ese1 ese1.cpp ese1_functions.cpp)