#version 440

//MVP uniform
uniform mat4 MVP;

//incoming position
layout(location = 0) in vec3 position;
//incoming tex coord
layout(location = 10) in vec2 tex_coord_in;

//outgoing tex coord
layout(location = 0) out vec2 tex_coord_out;

void main()
{
	//calculate screen position
	gl_Position = MVP * vec4(position, 1.0);

	//output tex_coord to fragment shader
	tex_coord_out = tex_coord_in;
} 