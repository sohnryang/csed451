add_gl_executable_single_file(setup_test_demo setup_test_demo.cpp)
add_gl_executable_single_file(lec02_rectangle lec02_rectangle.cpp)
add_gl_executable_single_file(lec02_rectangle_animation
                              lec02_rectangle_animation.cpp)
add_gl_executable_single_file(lec02_keyboard lec02_keyboard.cpp)
add_gl_executable_single_file(ecs_polygons ecs_polygons.cpp)
target_link_libraries(ecs_polygons ECS)
