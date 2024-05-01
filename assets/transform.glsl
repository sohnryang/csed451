#version 330 core

layout (location = 0) in vec3 pos;
uniform mat4 transform_mat;

void main() {
    gl_Position = transform_mat * vec4(pos.x, pos.y, pos.z, 1.0f);
}
