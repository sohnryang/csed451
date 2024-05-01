add_subdirectory(ecs)

add_executable(
  crossy_ponix
  game.cpp
  systems.cpp
  registry.cpp
  bounding_box.cpp
  grid.cpp
  scene.cpp
  model.cpp
  shader_program.cpp)
target_link_libraries(crossy_ponix OpenGL::GL GLUT::GLUT GLEW::glew ECS)
target_compile_definitions(crossy_ponix PRIVATE GL_SILENCE_DEPRECATION)
target_include_directories(crossy_ponix PRIVATE ${TINY_OBJ_LOADER_INCLUDE_DIR})
if(DEFINED GLM_INCLUDE_DIR)
  target_include_directories(crossy_ponix PRIVATE ${GLM_INCLUDE_DIR})
else()
  target_link_libraries(crossy_ponix glm::glm)
endif()
