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

out vec3 ambient_frag;
out vec3 diffuse_frag;
out vec3 specular_frag;
out vec2 tex_coord_frag;

vec3 diffuse_light(vec3 light_direction, vec3 normal, float intensity) {
  return max(dot(light_direction, normal), 0.0) * intensity * mat_diffuse;
}

vec3 specular_light(vec3 light_direction, vec4 pos_modelview, vec3 normal, float intensity) {
  vec3 eye = normalize(-pos_modelview.xyz);
  vec3 halfway = normalize(light_direction + eye);
  float ks = pow(max(dot(normal, halfway), 0.0), mat_shininess);
  vec3 specular = vec3(0.0, 0.0, 0.0);
  if (dot(light_direction, normal) >= 0.0)
    specular = intensity * ks * mat_specular;
  return specular;
}

void main() {
  vec4 pos_modelview = modelview_mat * vec4(pos, 1.0);
  gl_Position = projection_mat * pos_modelview;

  vec3 transformed_normal = normalize(modelview_mat * vec4(normal, 0.0)).xyz;
  vec3 point_light_direction = normalize(light_pos - pos_modelview.xyz);
  vec3 directional_light_direction = normalize(-directional_light);
  float inverse_square = 1 / (1 + pow(distance(light_pos, pos_modelview.xyz), 2));

  ambient_frag = ambient_intensity * mat_ambient;

  vec3 diffuse_point = inverse_square * diffuse_light(point_light_direction, transformed_normal, diffuse_intensity_point);
  vec3 diffuse_directional = diffuse_light(directional_light_direction, transformed_normal, diffuse_intensity_directional);
  diffuse_frag = diffuse_point + diffuse_directional;

  vec3 specular_point = inverse_square * specular_light(point_light_direction, pos_modelview, transformed_normal, specular_intensity_point);
  vec3 specular_directional = specular_light(directional_light_direction, pos_modelview, transformed_normal, specular_intensity_directional);
  specular_frag = specular_point + specular_directional;

  tex_coord_frag = tex_coord;
}
