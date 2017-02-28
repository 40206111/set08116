#version 440

// Spot light data
#ifndef SPOT_LIGHT
#define SPOT_LIGHT
struct spot_light
{
	vec4 light_colour;
	vec3 position;
	vec3 direction;
	float constant;
	float linear;
	float quadratic;
	float power;
};
#endif

// Material data
#ifndef MATERIAL
#define MATERIAL
struct material
{
	vec4 emissive;
	vec4 diffuse_reflection;
	vec4 specular_reflection;
	float shininess;
};
#endif

vec4 calculate_spot(in spot_light spot, in material mat, in vec3 vertex_pos, in vec3 transformed_normal, in vec3 view_dir, in vec4 tex_colour, in sampler1D shade)
{
	//calculate direction to light
	vec3 light_dir = normalize(spot.position - vertex_pos);
	//calculate distance to light
	float d = distance(spot.position, vertex_pos);
	//calculate attinuation value
	float att_fat = spot.constant * spot.linear * d + spot.quadratic * pow(d, 2);
	//calculate spot light intensity
	float intensity = max(dot(-light_dir, spot.direction), 0.0);
	//calculate spot tex_coords
	float shade_tex_coord = clamp(((pow(intensity, spot.power))/att_fat), 0.0, 1.0);
	//calculate light colour
	vec4 light_col = texture(shade, shade_tex_coord) * spot.light_colour;

	//calculate shade texture coords for diffuse
	shade_tex_coord = clamp(max(dot(transformed_normal, light_dir), 0.0),0.0, 1.0);
	//calculate diffuse
	vec4 diffuse = texture(shade, shade_tex_coord) * (mat.diffuse_reflection * light_col);
	//calculate half vector
	vec3 half_v = normalize(light_dir + view_dir);
	//calculate shade texture coords for specular
	shade_tex_coord = clamp(pow(max(dot(transformed_normal, half_v), 0.0),mat.shininess), 0.0, 1.0);
	//calculate specular
	vec4 specular =  texture(shade, shade_tex_coord) * (mat.specular_reflection * light_col);
	//caluclate colour
	vec4 colour =  ((mat.emissive + diffuse) * tex_colour) + specular;
	//insure colour alpha is 1
	colour.a = 1.0;

	return colour;
}