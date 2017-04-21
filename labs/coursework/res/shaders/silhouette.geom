#version 440

uniform mat4 MVP;
uniform vec3 cam_pos;

layout(triangles) in;

layout(line_strip, max_vertices = 6) out;
layout(location = 0) flat in vec3 normal[];

void main()
{
	for (int i = 0; i < 3; ++i)
	{
		vec3 vertex_pos = gl_in[i].gl_Position.xyz;
		vec3 normal = normalize(normal[i]);

		//if ((dot(normal, cam_pos)) > 0)
		//{
			gl_Position = MVP * vec4(vertex_pos,1.0) ;
			EmitVertex();
			gl_Position = MVP * vec4(vertex_pos + normal,1.0);
			EndPrimitive();
		//}
	}
}