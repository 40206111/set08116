#version 440

//texxture unform
uniform sampler2D tex;
//mask uniform
uniform sampler2D mask;
uniform vec3 filter_col;

//incoming tex coord
layout(location = 0) in vec2 tex_coord;

//outgoing colour
layout(location = 0) out vec4 colour;

void main() 
{
	//sample texture
	vec4 col1 = texture(tex, tex_coord);
	//sample mask
	vec4 col2 = texture(mask, tex_coord);

	//multiply texture colour by mask colour
	colour = col1 * col2;

	colour.rgb += filter_col;
	//ensure alpha is 1.0
	colour.a = 1.0;
}