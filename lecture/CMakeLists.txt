add_executable(setup_test_demo setup_test_demo.cpp)
target_link_libraries(setup_test_demo OpenGL::GL GLUT::GLUT GLEW::glew glm::glm)
