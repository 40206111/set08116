#version 440

vec3 calc_normal(in vec3 transformed_normal, in vec3 tangent, in vec3 binormal, in sampler2D normal_map, in vec2 tex_coord)
{
	transformed_normal = normalize(transformed_normal);
	tangent = normalize(tangent);
	binormal = normalize(binormal);

	vec3 sampled_normal = vec3(texture(normal_map, tex_coord));

	sampled_normal = 2.0 * sampled_normal - vec3(1.0, 1.0, 1.0);

	mat3 TBN = mat3(tangent, binormal, transformed_normal);

	return normalize(TBN * sampled_normal);
}