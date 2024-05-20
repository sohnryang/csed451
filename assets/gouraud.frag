#version 330 core

in vec3 ambient_frag;
in vec3 diffuse_frag;
in vec3 specular_frag;
in vec2 tex_coord_frag;

uniform sampler2D texture_sampler;
uniform int diffuse_on;

out vec4 FragColor;

void main() {
  FragColor = vec4(ambient_frag + specular_frag, 1.0) + 
	(diffuse_on > 0.0 ? vec4(diffuse_frag, 1.0) : vec4(1.0, 1.0, 1.0, 1.0)) * texture(texture_sampler, tex_coord_frag);
}
