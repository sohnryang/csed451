#version 330 core

in vec4 pos_modelview_frag;
in vec3 transformed_normal_frag;
in vec3 eye_frag;
in vec3 light_direction_frag;
in vec3 ambient_frag;
in vec3 diffuse_product_point_frag;
in vec3 diffuse_product_directional_frag;
in float mat_shininess_frag;
in vec3 specular_product_point_frag;
in vec3 specular_product_directional_frag;
in vec2 tex_coord_frag;

uniform vec3 light_pos;
uniform vec3 directional_light;
uniform sampler2D texture_sampler;
uniform int diffuse_on;

out vec4 FragColor;

vec3 diffuse_light(vec3 light_direction, vec3 normal, vec3 product) {
  return max(dot(light_direction, normal), 0.0) * product;
}

vec3 specular_light(vec3 light_direction, vec4 pos_modelview, vec3 normal, vec3 product) {
  vec3 eye = normalize(-pos_modelview.xyz);
  vec3 halfway = normalize(light_direction + eye);
  float ks = pow(max(dot(normal, halfway), 0.0), mat_shininess_frag);
  vec3 specular = vec3(0.0, 0.0, 0.0);
  if (dot(light_direction, normal) >= 0.0)
    specular = ks * product;
  return specular;
}

void main() {
  vec3 transformed_normal = normalize(transformed_normal_frag);
  vec3 eye = normalize(eye_frag);
  vec3 light_direction = normalize(light_direction_frag);
  vec3 halfway = normalize(light_direction + eye);

  float inverse_square = 1 / (1 + pow(distance(light_pos, pos_modelview_frag.xyz), 2));
  vec3 diffuse_point = inverse_square * diffuse_light(light_direction, transformed_normal, diffuse_product_point_frag);
  vec3 directional_light_direction = normalize(-directional_light);
  vec3 diffuse_directional = diffuse_light(directional_light_direction, transformed_normal, diffuse_product_directional_frag);
  vec3 diffuse = diffuse_point + diffuse_directional;

  vec3 specular_point = inverse_square * specular_light(light_direction, pos_modelview_frag, transformed_normal, specular_product_point_frag);
  vec3 specular_directional = specular_light(directional_light_direction, pos_modelview_frag, transformed_normal, specular_product_directional_frag);
  vec3 specular = specular_point + specular_directional;

  vec3 shaded = ambient_frag + diffuse + specular;
  FragColor = vec4(ambient_frag + specular, 1.0) + 
	(diffuse_on > 0.0 ? vec4(diffuse, 1.0) : vec4(1.0, 1.0, 1.0, 1.0)) * texture(texture_sampler, tex_coord_frag);
}
