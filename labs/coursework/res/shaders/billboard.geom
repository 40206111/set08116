#version 440

uniform mat4 P;
uniform float point_size;

layout(points) in;

layout(triangle_strip, max_vertices = 6) out;
layout(location = 0) out vec2 tex_coord;

void main()
{
    vec4 position = gl_in[0].gl_Position;

	vec2 va = position.xy + vec2(-0.5, -0.5) * point_size;
	gl_Position = P * vec4(va, position.zw);
	tex_coord = vec2(0.0, 0.0);
	EmitVertex();

	va = position.xy + vec2(0.5, -0.5) * point_size;
	gl_Position = P * vec4(va, position.zw);
	tex_coord = vec2(1.0, 0.0);
	EmitVertex();

	va = position.xy + vec2(-0.5, 0.5) * point_size;
	gl_Position = P * vec4(va, position.zw);
	tex_coord = vec2(0.0, 1.0);
	EmitVertex();

	va = position.xy + vec2(0.5, 0.5) * point_size;
	gl_Position = P * vec4(va, position.zw);
	tex_coord = vec2(1.0, 1.0);
	EmitVertex();
	EndPrimitive();
}