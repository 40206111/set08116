#version 440

uniform sampler2D tex;
uniform sampler1D shade_tex;

layout(location = 0) in vec2 tex_coord;

layout(location = 2) in vec3 transformed_normal;

layout(location = 0) out vec4 colour;

const vec3 light_dir = vec3(-1.0, 0.0, 0.0);

void main() {


float shade_tex_coord = clamp(max(0.0, dot(normalize(transformed_normal), light_dir)), 0.0, 1.0);
colour = texture(tex, tex_coord) * texture(shade_tex, shade_tex_coord);
}