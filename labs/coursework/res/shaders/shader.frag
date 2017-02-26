#version 440

uniform sampler2D tex;
uniform sampler1D shade_tex;

layout(location = 0) in vec2 tex_coord;

layout(location = 1) in float shade_tex_coord;

layout(location = 0) out vec4 colour;

void main() {

colour = texture(tex, tex_coord) * texture(shade_tex, shade_tex_coord);
}