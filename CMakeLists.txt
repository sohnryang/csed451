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

if (WIN32)
  configure_file("${GLUT_LIBRARIES}/../../bin/freeglut.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/freeglut.dll" COPYONLY)
  configure_file("${GLEW_LIBRARIES}/../../../../bin/Release/Win32/glew32.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/glew32.dll" COPYONLY)
endif()

add_subdirectory(lecture)
