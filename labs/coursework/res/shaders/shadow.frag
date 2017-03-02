#version 440

float calculate_shadow(in sampler2D shadow_map, in vec4 light_space_pos)
{
	vec3 proj_coords = light_space_pos.xyz / light_space_pos.w;

	vec2 shadow_tex_coords;

	shadow_tex_coords.x = 0.5 * proj_coords.x + 0.5;
	shadow_tex_coords.y = 0.5 * proj_coords.y + 0.5;

	if (shadow_tex_coords.x < 0 || shadow_tex_coords.x > 1 || shadow_tex_coords.y < 0 || shadow_tex_coords.y > 1)
	{
	return 1.0;
	}

	float z = light_space_pos.z / light_space_pos.w;

	float depth = texture(shadow_map, shadow_tex_coords).x;

	if (depth == 0.0)
	{
		return 1.0;
	} else if (depth < z + 0.001)
	{
	return 0.5;
	} else
	{
	return 1.0;
	}
}