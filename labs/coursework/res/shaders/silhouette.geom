#version 440

//incoming MVP uniform
uniform mat4 MVP;
//incoming model matrix uniform
uniform mat4 M;
//incoming camera position uniform
uniform vec3 cam_pos;
//incoming line width uniform
uniform float line_width;

//incoming triangles
layout(triangles) in;
//incoming normals
layout(location = 0) flat in vec3 normal[];

//outgoing triangle strip
layout(triangle_strip, max_vertices = 9) out;

void main()
{
	//arrays to store front and back facing normals
    int fFace[3];
	int bFace[3];
	//int to store number of front facing normals
    int j = 0;
	//int to store number of back facing normals
    int k = 0;

	//loop through triangle vertices
    for (int i = 0; i < 3; ++i)
	{
		//set vertex
        vec3 vertex_pos = gl_in[i].gl_Position.xyz;
		//normalize normal
        vec3 norm = normalize(normal[i]);
		//get vector from vertex to camera
        vec4 ca = vec4(cam_pos, 1.0) - (M * vec4(vertex_pos,1.0));

		//Check if vertex normal is front facing
        if ((dot((M * vec4(norm, 1.0)).xyz, ca.xyz)) > 0.0)
		{
			//add vertex index to front face variable
            fFace[j] = i;
			//increment number of front facing normals
            j++;
        }
		else
		{
			//add vertex index to back face variable
            bFace[k] = i;
			//increment number of back facing normals
            k++;
        }

	}

	//check if there's 1 or 2 front facing normals
	if (j == 1)
	{
		float a = dot((M * vec4(normal[fFace[0]], 1.0)).xyz, (cam_pos - (M * gl_in[fFace[0]].gl_Position).xyz));
		float b1 = dot((M * vec4(normal[bFace[0]], 1.0)).xyz, (cam_pos - (M * gl_in[bFace[0]].gl_Position).xyz));
		float b2 = dot((M * vec4(normal[bFace[1]], 1.0)).xyz, (cam_pos - (M * gl_in[bFace[1]].gl_Position).xyz));

		//calculate intersectionpoint
		vec4 yab1 = (a * gl_in[bFace[0]].gl_Position - b1 * gl_in[fFace[0]].gl_Position)/(a - b1);
		vec4 yab2 = (a * gl_in[bFace[1]].gl_Position - b2 * gl_in[fFace[0]].gl_Position)/(a - b2);

		//find normal of new point
		vec3 norm = cross((yab2 - yab1).xyz, (gl_in[bFace[0]].gl_Position - yab1).xyz);
		norm = normalize(norm);

		//emit silhouette quad
        gl_Position = MVP * yab1;
        EmitVertex();
        gl_Position = MVP * yab2;
        EmitVertex();
		gl_Position = MVP * vec4(yab1.xyz + norm * line_width, 1.0);
        EmitVertex();
		gl_Position = MVP * vec4(yab2.xyz + norm * line_width, 1.0);
        EmitVertex();
    }
	else if (j == 2)
	{
		float a1 = dot((M * vec4(normal[fFace[0]], 1.0)).xyz, (cam_pos - (M * gl_in[fFace[0]].gl_Position).xyz));
		float a2 = dot((M * vec4(normal[fFace[1]], 1.0)).xyz, (cam_pos - (M * gl_in[fFace[1]].gl_Position).xyz));
		float b = dot((M * vec4(normal[bFace[0]], 1.0)).xyz, (cam_pos - (M * gl_in[bFace[0]].gl_Position).xyz));

		//calculate intersectionpoint
		vec4 yab1 = (a1 * gl_in[bFace[0]].gl_Position - b * gl_in[fFace[0]].gl_Position)/(a1 - b);
		vec4 yab2 = (a2 * gl_in[bFace[0]].gl_Position - b * gl_in[fFace[1]].gl_Position)/(a2 - b);
		
		//find normal of new point
		vec3 norm = cross((yab2 - yab1).xyz, (gl_in[bFace[0]].gl_Position - yab1).xyz);
		norm = normalize(norm);

		//emit silhouette quad
        gl_Position = MVP * yab1;
        EmitVertex();
        gl_Position = MVP * yab2;
        EmitVertex();
		gl_Position = MVP * vec4(yab1.xyz + norm * line_width, 1.0);
        EmitVertex();
		gl_Position = MVP * vec4(yab2.xyz + norm * line_width, 1.0);
        EmitVertex();
    }
	EndPrimitive();
}