#version 440

//texture uniform
uniform sampler2D tex;

//incoming tex coord
layout(location = 0) in vec2 tex_coord;

//outgoing colour
layout(location = 0) out vec4 out_colour;

void main() {

	//colour of silhouette is black
	out_colour = vec4(0.0,1.0,0.0,1.0);
}