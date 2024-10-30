cmake_minimum_required (VERSION 3.20)
set(CMAKE_C_COMPILER /usr/bin/clang)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)
set(CMAKE_CXX_STANDARD 17)

project(lunarenderer)

set(SOURCE
    model.cpp
    triangle.cpp
    rasterizer.cpp
    tgaimage.cpp
    shader.cpp
    main.cpp)

include_directories("/opt/homebrew/opt")

add_executable(${PROJECT_NAME} ${SOURCE})