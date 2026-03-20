#specifichiamo che versione di CMake utilizzare
cmake_minimum_required(VERSION 3.20)

#specifichiamo il nome del progetto
project(ese2)

#specifichaimo la versione di c++ da utilizzare
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#incrementiamo i livelli di Warning
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wshadow -Werror=vla")

#dichiariamo l'eseguibile
add_executable(ese2 ese2.cpp ese2_func.cpp)