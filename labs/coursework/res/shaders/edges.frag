#version 440

uniform sampler2D tex;
uniform float inverse_width;
uniform float inverse_height;

const vec3 samples[6] = vec3[6](vec3(-1.0, 1.0, 1.0), vec3(0.0, 1.0, 2.0), vec3(1.0, 1.0, 1.0), vec3(-1.0, -1.0, -1.0), vec3(-1.0, 0.0, -2.0), vec3(1.0, -1.0, -1.0));

layout(location = 0) in vec2 tex_coord;
layout(location = 0) out vec4 colour;

void main()
{
    colour = vec4(vec3(0.0), 1.0);
    for (int i = 0; i < 6; ++i)
	{
        vec2 uv = tex_coord + vec2(samples[i].x * inverse_width, samples[i].y * inverse_height);
        colour += texture(tex, uv) * samples[i].z;
    }


	if (colour.r <= 0.3 && colour.g <= 0.3 && colour.b <= 0.3)
	{
	    colour = vec4(1.0);
	}
	else
	{
	    colour = vec4(0.0,0.0,0.0,1.0);
	}

}