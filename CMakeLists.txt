cmake_minimum_required(VERSION 3.10)

project(3DCellularAutomaton)

set(CMAKE_CXX_STANDART 20)
set(CMAKE_CXX_STANDART_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")

find_package(OpenGL REQUIRED)

set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")

include_directories("${PROJECT_SOURCE_DIR}/include")
link_directories("${PROJECT_SOURCE_DIR}/lib")

file(GLOB_RECURSE SOURCES "${SOURCE_DIR}/*.cpp")

add_executable(3DCellularAutomaton ${SOURCES})

if (WIN32)
target_link_libraries(3DCellularAutomaton glfw3 opengl32 glew32s)
elseif (UNIX)
target_link_libraries(3DCellularAutomaton glfw3 GL GLEW)
endif()