#version 440

uniform mat4 MVP;
uniform vec3 cam_pos;
uniform float line_width;

layout(triangles) in;
layout(location = 0) flat in vec3 normal[];

layout(triangle_strip, max_vertices = 6) out;

void main()
{
    vec3 vertex_pos;
    vec3 norm;
    int fFace[3];
    int bFace[3];
    int j = 0;
    int k = 0;

    for (int i = 0; i < 3; ++i)
	{
        vertex_pos = gl_in[i].gl_Position.xyz;
        norm = normalize(normal[i]);
        vec4 ca = vec4(cam_pos, 1.0) - (MVP * vec4(vertex_pos,1.0));
        if ((dot(norm, ca.xyz)) > 0.0)
		{
            fFace[j] = i;
            j++;
        }

		else
		{
            bFace[k] = i;
            k++;
        }

	}

	if (j == 1)
	{
		float a = dot(normal[fFace[0]], (cam_pos - (MVP * gl_in[fFace[0]].gl_Position).xyz));
		float b1 = dot(normal[bFace[0]], (cam_pos - (MVP * gl_in[bFace[0]].gl_Position).xyz));
		float b2 = dot(normal[bFace[1]], (cam_pos - (MVP * gl_in[bFace[1]].gl_Position).xyz));

		vec4 yab1 = (a * gl_in[bFace[0]].gl_Position - b1 * gl_in[fFace[0]].gl_Position)/(a - b1);
		vec4 yab2 = (a * gl_in[bFace[1]].gl_Position - b2 * gl_in[fFace[0]].gl_Position)/(a - b2);

		vec3 crx = cross((yab2 - yab1).xyz, (gl_in[bFace[0]].gl_Position - yab1).xyz);
		crx = normalize(crx);

        gl_Position = MVP * yab1;
        EmitVertex();
        gl_Position = MVP * yab2;
        EmitVertex();
		gl_Position = MVP * vec4(yab1.xyz + crx, 1.0);
        EmitVertex();
		gl_Position = MVP * vec4(yab2.xyz + crx, 1.0);
        EmitVertex();
    }
	else if (j == 2)
	{
		float a1 = dot(normal[fFace[0]], (cam_pos - (MVP * gl_in[fFace[0]].gl_Position).xyz));
		float a2 = dot(normal[fFace[1]], (cam_pos - (MVP * gl_in[fFace[1]].gl_Position).xyz));
		float b = dot(normal[bFace[0]], (cam_pos - (MVP * gl_in[bFace[0]].gl_Position).xyz));

		vec4 yab1 = (a1 * gl_in[bFace[0]].gl_Position - b * gl_in[fFace[0]].gl_Position)/(a1 - b);
		vec4 yab2 = (a2 * gl_in[bFace[0]].gl_Position - b * gl_in[fFace[1]].gl_Position)/(a2 - b);
		
		vec3 crx = cross((yab2 - yab1).xyz, (gl_in[bFace[0]].gl_Position - yab1).xyz);
		crx = normalize(crx);

        gl_Position = MVP * yab1;
        EmitVertex();
        gl_Position = MVP * yab2;
        EmitVertex();
		gl_Position = MVP * vec4(yab1.xyz + crx, 1.0);
        EmitVertex();
		gl_Position = MVP * vec4(yab2.xyz + crx, 1.0);
        EmitVertex();
    }
	EndPrimitive();
}