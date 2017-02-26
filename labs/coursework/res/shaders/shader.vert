#version 440

uniform mat4 MVP;

layout(location = 0) in vec3 position;
layout(location = 2) in vec3 normal;

layout(location = 10) in vec2 tex_coord_in;

layout (location = 0) out vec2 tex_coord;

layout (location = 1) out float shade_tex_coord;

const vec3 light_dir = vec3(0, 0, 1.0);

void main()
{
	gl_Position = MVP * vec4(position,1.0);
	tex_coord = tex_coord_in;
	shade_tex_coord = clamp(max(0.0, dot(normalize(normal), light_dir)), 0.0, 1.0);
} 