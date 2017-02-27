#version 440

//define directional light
#ifndef DIRECTIONAL_LIGHT
#define DIRECTIONAL-LIGHT
struct directional_light {
	vec4 ambient_intensity;
	vec4 light_colour;
	vec3 light_dir;
};
#endif

//define material
#ifndef MATERIAL
#define MATERIAL
struct material {
	vec4 emissive;
	vec4 diffuse_reflection;
	vec4 specular_reflection;
	float shininess;
}; 
#endif

//Forward declarations of used functions
vec4 calculate_direction(in directional_light light, in material mat, in vec3 transformed_normal, in vec3 view_dir, in vec4 tex_colour, in sampler1D shade);

//texture uniform
uniform sampler2D tex;
//shade uniform
uniform sampler1D shade_tex;
//directional light information
uniform directional_light light;
//position of the eye
uniform vec3 eye_pos;
//material of the object being rendered
uniform material mat;

//incoming position
layout(location = 0) in vec3 vertex_pos;
//incoming texture coordinate
layout(location = 1) in vec2 tex_coord;
//incoming normal
layout(location = 2) in vec3 transformed_normal;
//outgoing colour
layout(location = 0) out vec4 colour;

void main() {

//calculate view direction
vec3 view_dir = normalize(eye_pos - vertex_pos);
//sample texture
vec4 tex_colour = texture(tex, tex_coord);
//calculate directional light colour
colour += calculate_direction(light, mat, transformed_normal, view_dir, tex_colour, shade_tex);

//float shade_tex_coord = clamp(max(0.0, dot(normalize(transformed_normal), light.light_dir)), 0.0, 1.0);
//colour = tex_colour * texture(shade_tex, shade_tex_coord);
}