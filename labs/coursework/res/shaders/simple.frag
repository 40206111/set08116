#version 440

uniform sampler2D tex;

layout(location = 0) in vec2 tex_coord;

layout(location = 0) out vec4 out_colour;

void main() {

	out_colour = texture(tex, tex_coord);
}