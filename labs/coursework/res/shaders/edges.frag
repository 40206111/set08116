#version 440

//texture uniform
uniform sampler2D tex;
//inverse width and height uniforms
uniform float inverse_width;
uniform float inverse_height;

//surrounding pixels to sample their scale
const vec3 samples[6] = vec3[6](vec3(-1.0, 1.0, 1.0), vec3(0.0, 1.0, 2.0), vec3(1.0, 1.0, 1.0), vec3(-1.0, -1.0, -1.0), vec3(-1.0, 0.0, -2.0), vec3(1.0, -1.0, -1.0));

//incoming tex coord
layout(location = 0) in vec2 tex_coord;
//outgoing colour
layout(location = 0) out vec4 colour;

void main()
{
	//loop through each sample vector
    for (int i = 0; i < 6; ++i)
	{
		//calculate tex coord to sample
        vec2 uv = tex_coord + vec2(samples[i].x * inverse_width, samples[i].y * inverse_height);
		//sample the texture and scale appropriately
        colour += texture(tex, uv) * samples[i].z;
    }

	//check if colour black or not
	if (colour.r <= 0.07 && colour.g <= 0.07 && colour.b <= 0.07)
	{
		//if colour black make white
	    colour = vec4(1.0);
	}
	else
	{
		//else make colour black
	    colour = vec4(0.0,0.0,0.0,1.0);
	}
	
	//FOR TESTING SILHOUETTING
	   colour = vec4(1.0);
}