#version 440

uniform mat4 MVP;
uniform vec3 cam_pos;

layout(triangles) in;

layout(line_strip, max_vertices = 6) out;
layout(location = 0) flat in vec3 normal[];

void main()
{

	vec3 vertex_pos;
	vec3 norm;

	int j = 0;
	for (int i = 0; i < 3; ++i)
	{
		vertex_pos = gl_in[i].gl_Position.xyz;
		norm = normalize(normal[i]);	

		vec4 ca = vec4(cam_pos, 1.0) - (MVP * vec4(vertex_pos,1.0));

		if ((dot(norm, ca.xyz)) > 0.0)
		{
			j++;
		}
	}

	if (j == 1 || j ==2)
	{
		gl_Position = MVP * vec4(vertex_pos, 1.0);
		EmitVertex();
		gl_Position = MVP * vec4(vertex_pos + norm, 1.0);
		EmitVertex();
		EndPrimitive();
	}
}