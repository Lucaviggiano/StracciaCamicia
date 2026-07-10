cmake_minimum_required(VERSION 3.10)
project(StracciaCamicia_Sim)

# Forza l'output dell'eseguibile direttamente nella cartella build/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

# Richiediamo lo standard C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Flag di ottimizzazione e warning per pulizia del codice
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -march=native")
endif()

# Diciamo a CMake dove trovare i file .hpp
include_directories(include)

# Cerchiamo la libreria standard per i thread (pthread su Linux)
find_package(Threads REQUIRED)

# Definiamo i file sorgente che compongono l'eseguibile
set(SOURCES
    src/main.cpp
    src/Pipeline.cpp
    src/FastKernel.cpp
    src/CycleInspector.cpp
)

# Creiamo l'eseguibile
add_executable(StracciaCamicia ${SOURCES})

# Colleghiamo la libreria dei thread all'eseguibile
target_link_libraries(StracciaCamicia Threads::Threads)