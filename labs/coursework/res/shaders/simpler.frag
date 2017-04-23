#version 440

//incoming colour uniform
uniform vec4 col;

//outgoing colour
layout(location = 0) out vec4 out_colour;

void main() {

	//set colour
	out_colour = col;
}