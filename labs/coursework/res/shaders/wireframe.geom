#version 440

//incoming MVP uniform
uniform mat4 MVP;

//incoming triangles
layout(triangles) in;
//incoming normals
layout(location = 0) flat in vec3 normal[];

//outgoing line strip
layout(line_strip, max_vertices = 3) out;

void main()
{
	//loop through vertices
    for (int i = 0; i < 3; ++i)
	{
		//set vertex position
        vec3 vertex_pos = gl_in[i].gl_Position.xyz;
		//normalize vertex normal
        vec3 norm = normalize(normal[i]);
		
		//emit vertex at position
		gl_Position = MVP * vec4(vertex_pos, 1.0);
        EmitVertex();
	}
		EndPrimitive();
}