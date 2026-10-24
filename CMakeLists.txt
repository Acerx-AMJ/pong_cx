cmake_minimum_required(VERSION 3.10)
project(pong)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

find_package(cx REQUIRED)
find_package(SFML 2.6 COMPONENTS window system graphics audio REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE cx sfml-window sfml-system sfml-graphics sfml-audio)
