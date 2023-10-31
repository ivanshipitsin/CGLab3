cmake_minimum_required(VERSION 3.7)

project(GRAPH VERSION 1.0
            LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(ImageMagick COMPONENTS Magick++ MagickCore)
find_package(Armadillo REQUIRED)

add_executable(graph main.cpp graphics.cpp curve.cpp geometry.cpp)

include_directories(${ARMADILLO_INCLUDE_DIRS})
target_link_libraries(graph PRIVATE ImageMagick::Magick++ ImageMagick::MagickCore  ${ARMADILLO_LIBRARIES})
