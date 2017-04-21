#version 440

//texture uniform
uniform sampler2D tex;

//incoming tex coord
layout(location = 0) in vec2 tex_coord;

//outgoing colour
layout(location = 0) out vec4 out_colour;

void main() {

	//sample texture
	out_colour = texture(tex, tex_coord);
}