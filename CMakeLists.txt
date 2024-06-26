cmake_minimum_required(VERSION 3.19)
project(CSED451)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

include(FetchContent)

find_package(OpenGL REQUIRED)

if(FETCH_GLUT)
  FetchContent_Declare(
    GLUT::GLUT
    GIT_REPOSITORY https://github.com/freeglut/freeglut
    GIT_TAG d35af9686a6c11b5ed1629a054fccc4414ba9854
    OVERRIDE_FIND_PACKAGE)
  find_package(GLUT::GLUT REQUIRED)
else()
  find_package(GLUT REQUIRED)
endif()

if(FETCH_GLEW)
  FetchContent_Declare(
    GLEW::GLEW
    GIT_REPOSITORY https://github.com/nigels-com/glew
    GIT_TAG 3a8eff77da3658c13fbd3634c943d5251d76322c
    OVERRIDE_FIND_PACKAGE)
  find_package(GLEW::GLEW REQUIRED)
else()
  find_package(GLEW REQUIRED)
endif()

if(NOT DEFINED GLM_INCLUDE_DIR)
  if(FETCH_GLM)
    FetchContent_Declare(
      glm::glm
      GIT_REPOSITORY https://github.com/g-truc/glm
      GIT_TAG 13724cfae64a8b5313d1cabc9a963d2c9dbeda12
      OVERRIDE_FIND_PACKAGE)
    find_package(glm::glm REQUIRED)
  else()
    find_package(glm REQUIRED)
  endif()
endif()

if(WIN32)
  configure_file("${GLUT_LIBRARIES}/../../bin/freeglut.dll"
                 "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/freeglut.dll" COPYONLY)
  configure_file("${GLEW_LIBRARIES}/../../../../bin/Release/Win32/glew32.dll"
                 "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/glew32.dll" COPYONLY)
endif()

if(ASAN)
  add_compile_options(-fsanitize=address)
  add_link_options(-fsanitize=address)
endif()

function(add_gl_executable_single_file exec_name source_file)
  add_executable(${exec_name} ${source_file})
  target_link_libraries(${exec_name} OpenGL::GL GLUT::GLUT GLEW::glew)
  target_compile_definitions(${exec_name} PRIVATE GL_SILENCE_DEPRECATION)
  if(DEFINED GLM_INCLUDE_DIR)
    target_include_directories(${exec_name} PRIVATE ${GLM_INCLUDE_DIR})
  else()
    target_link_libraries(${exec_name} glm::glm)
  endif()
endfunction()

set(TINY_OBJ_LOADER_INCLUDE_DIR
    "${PROJECT_SOURCE_DIR}/third_party/tinyobjloader")
set(STB_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/third_party/stb")
set(ASSETS_DIR "${PROJECT_SOURCE_DIR}/assets")
file(
  COPY "${ASSETS_DIR}/teapot.obj"
       "${ASSETS_DIR}/teapot.mtl"
       "${ASSETS_DIR}/rooster.obj"
       "${ASSETS_DIR}/rooster.mtl"
       "${ASSETS_DIR}/rooster_texture.jpg"
       "${ASSETS_DIR}/tree.obj"
       "${ASSETS_DIR}/tree.mtl"
       "${ASSETS_DIR}/tree_texture.png"
       "${ASSETS_DIR}/car.obj"
       "${ASSETS_DIR}/car.mtl"
       "${ASSETS_DIR}/car_texture.png"
       "${ASSETS_DIR}/truck.obj"
       "${ASSETS_DIR}/truck.mtl"
       "${ASSETS_DIR}/truck_texture.jpg"
       "${ASSETS_DIR}/sneakers.obj"
       "${ASSETS_DIR}/sneakers.mtl"
       "${ASSETS_DIR}/sneakers_normal.jpg"
       "${ASSETS_DIR}/floor.obj"
       "${ASSETS_DIR}/floor.mtl"
       "${ASSETS_DIR}/floor2.obj"
       "${ASSETS_DIR}/floor2.mtl"
       "${ASSETS_DIR}/ground_texture.jpg"
       "${ASSETS_DIR}/ground_normal.jpg"
       "${ASSETS_DIR}/road_texture.jpg"
       "${ASSETS_DIR}/road_normal.png"
       "${ASSETS_DIR}/empty_texture.png"
       "${ASSETS_DIR}/transform.glsl"
       "${ASSETS_DIR}/wireframe.glsl"
       "${ASSETS_DIR}/gouraud.vert"
       "${ASSETS_DIR}/gouraud.frag"
       "${ASSETS_DIR}/phong.vert"
       "${ASSETS_DIR}/phong.frag"
       "${ASSETS_DIR}/empty_normal.png"
  DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

add_subdirectory(src)
add_subdirectory(examples)
