#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 mat_ambient;
layout (location = 3) in vec3 mat_diffuse;
layout (location = 4) in vec3 mat_specular;
layout (location = 5) in float mat_shininess;
layout (location = 6) in vec2 tex_coord;

uniform vec3 light_pos;
uniform vec3 directional_light;
uniform float ambient_intensity;
uniform float diffuse_intensity_point;
uniform float specular_intensity_point;
uniform float diffuse_intensity_directional;
uniform float specular_intensity_directional;
uniform mat4 projection_mat;
uniform mat4 modelview_mat;

out vec4 pos_modelview_frag;
out vec3 transformed_normal_frag;
out vec3 eye_frag;
out vec3 light_direction_frag;
out vec3 ambient_frag;
out vec3 diffuse_product_point_frag;
out vec3 diffuse_product_directional_frag;
out float mat_shininess_frag;
out vec3 specular_product_point_frag;
out vec3 specular_product_directional_frag;
out vec2 tex_coord_frag;

void main() {
  pos_modelview_frag = modelview_mat * vec4(pos, 1.0);
  gl_Position = projection_mat * pos_modelview_frag;

  transformed_normal_frag = normalize(modelview_mat * vec4(normal, 0.0)).xyz;
  light_direction_frag = normalize(light_pos - pos_modelview_frag.xyz);
  eye_frag = normalize(-pos_modelview_frag.xyz);

  ambient_frag = ambient_intensity * mat_ambient;
  diffuse_product_point_frag = diffuse_intensity_point * mat_diffuse;
  diffuse_product_directional_frag = diffuse_intensity_directional * mat_diffuse;
  mat_shininess_frag = mat_shininess;
  specular_product_point_frag = specular_intensity_point * mat_specular;
  specular_product_directional_frag = specular_intensity_directional * mat_specular;

  tex_coord_frag = tex_coord;
}
