cmake_minimum_required(VERSION 3.14)
project(MyProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Ofast -g -DNDEBUG)

add_executable(ctos 
    src/main.cpp
    src/tree_of_shapes.cpp  
    src/component_tree.cpp
    src/node_tos.cpp
    src/graph_of_shapes.cpp
    src/complete_tree_of_shapes.cpp
    src/node_ct.cpp
    src/benchmark.cpp
)

target_include_directories(ctos
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    /home/mendes/Downloads/Higra/include
)

set(OpenCV_DIR "/usr/lib/x86_64-linux-gnu/cmake/opencv4")
find_package(OpenCV REQUIRED)

target_include_directories(ctos PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_libraries(ctos PRIVATE ${OpenCV_LIBS})