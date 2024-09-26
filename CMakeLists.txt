cmake_minimum_required (VERSION 3.20)
project(lunarenderer)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE
    model.cpp
    triangle.cpp
    rasterizer.cpp
    tgaimage.cpp
    shader.cpp
    main.cpp)

include_directories("C:/Program Files (x86)/Eigen3/include/eigen3")

add_executable(${PROJECT_NAME} ${SOURCE})