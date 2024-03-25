add_subdirectory(ecs)

add_executable(crossy_ponix game.cpp components.cpp systems.cpp registry.cpp
                            bounding_box.cpp grid.cpp)
target_link_libraries(crossy_ponix OpenGL::GL GLUT::GLUT GLEW::glew ECS)
target_compile_definitions(crossy_ponix PRIVATE GL_SILENCE_DEPRECATION)
if(DEFINED GLM_INCLUDE_DIR)
  target_include_directories(crossy_ponix PRIVATE ${GLM_INCLUDE_DIR})
else()
  target_link_libraries(crossy_ponix glm::glm)
endif()
