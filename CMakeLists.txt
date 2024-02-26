cmake_minimum_required(VERSION 3.19)
project(CSED451)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
if (NOT DEFINED GLM_INCLUDE_DIR)
  find_package(glm REQUIRED)
endif()

add_subdirectory(lecture)
