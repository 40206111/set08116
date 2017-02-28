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

//define spotlight
#ifndef SPOT_LIGHT
#define SPOT_LIGHT
struct spot_light
{
	vec4 light_colour;
	vec3 position;
	vec3 direction;
	float constant;
	float linear;
	float quadratic;
	float power;
};
#endif

//define point light
#ifndef POINT_LIGHT
#define POINT_LIGHT
struct point_light
{
	vec4 light_colour;
	vec3 position;
	float constant;
	float linear;
	float quadratic;
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
vec4 calculate_spot(in spot_light spot, in material mat, in vec3 position, in vec3 view_dir, in vec3 transformed_normal, in vec4 tex_colour, in sampler1D shade);
vec4 calculate_point(in point_light point, in material mat, in vec3 vertex_pos, in vec3 transformed_normal, in vec3 view_dir, in vec4 tex_colour, in sampler1D shade);
float calculate_shadow(in sampler2D shadow_map, in vec4 light_space_pos);

//texture uniform
uniform sampler2D tex;
//shade uniform
uniform sampler1D shade_tex;
//directional light information
uniform directional_light light;
//spot light information
uniform spot_light spot;
//point light information
uniform point_light point;
//position of the eye
uniform vec3 eye_pos;
//material of the object being rendered
uniform material mat;
//shadow map
uniform sampler2D shadow_map;

//incoming position
layout(location = 0) in vec3 vertex_pos;
//incoming texture coordinate
layout(location = 1) in vec2 tex_coord;
//incoming normal
layout(location = 2) in vec3 transformed_normal;
//incoming light space position
layout(location = 3) in vec4 light_space_pos;

//outgoing colour
layout(location = 0) out vec4 colour;

void main() {

//calculate shadow factor
float shadow = calculate_shadow(shadow_map, light_space_pos);

//calculate view direction
vec3 view_dir = normalize(eye_pos - vertex_pos);
//sample texture
vec4 tex_colour = texture(tex, tex_coord);
//calculate directional light colour
colour += calculate_direction(light, mat, transformed_normal, view_dir, tex_colour, shade_tex);

//calculate spot light colour
colour += calculate_spot(spot, mat, vertex_pos, transformed_normal,  view_dir, tex_colour, shade_tex);

//calculate point light colour
colour += calculate_point(point, mat, vertex_pos, transformed_normal, view_dir, tex_colour, shade_tex);

colour *= shadow;
colour.a = 1.0f;
}