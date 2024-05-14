add_gl_executable_single_file(setup_test_demo setup_test_demo.cpp)
add_gl_executable_single_file(lec02_rectangle lec02_rectangle.cpp)
add_gl_executable_single_file(lec02_rectangle_animation
                              lec02_rectangle_animation.cpp)
add_gl_executable_single_file(lec02_keyboard lec02_keyboard.cpp)
add_gl_executable_single_file(ecs_polygons ecs_polygons.cpp)
target_link_libraries(ecs_polygons ECS)
add_gl_executable_single_file(teapot teapot.cpp)
target_include_directories(teapot PRIVATE ${TINY_OBJ_LOADER_INCLUDE_DIR})
add_gl_executable_single_file(teapot_shader teapot_shader.cpp)
target_include_directories(teapot_shader PRIVATE ${TINY_OBJ_LOADER_INCLUDE_DIR})
add_gl_executable_single_file(texture_mapping texture_mapping.cpp)
target_include_directories(
  texture_mapping
  PRIVATE ${TINY_OBJ_LOADER_INCLUDE_DIR}
  PRIVATE ${STB_INCLUDE_DIR})
add_gl_executable_single_file(gouraud_shading gouraud_shading.cpp)
target_include_directories(
  gouraud_shading
  PRIVATE ${TINY_OBJ_LOADER_INCLUDE_DIR}
  PRIVATE ${STB_INCLUDE_DIR})
add_gl_executable_single_file(phong_shading phong_shading.cpp)
target_include_directories(
  phong_shading
  PRIVATE ${TINY_OBJ_LOADER_INCLUDE_DIR}
  PRIVATE ${STB_INCLUDE_DIR})
