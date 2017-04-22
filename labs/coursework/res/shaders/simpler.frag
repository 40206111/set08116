#version 440

//outgoing colour
layout(location = 0) out vec4 out_colour;

void main() {

	//colour of silhouette is black
	out_colour = vec4(0.0,0.0,0.0,1.0);
}