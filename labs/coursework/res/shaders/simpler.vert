#version 440

//incoming position
layout(location = 0) in vec3 position;
//incoming tex coord
layout(location = 10) in vec3 normal;

// Outgoing normal
layout (location = 0)  out vec3 normal_out;

void main()
{
	//calculate screen position
	gl_Position = vec4(position, 1.0);
	// Output normal
	normal_out = normal;
} 