#version 440

//define directional light
#ifndef DIRECTIONAL_LIGHT
#define DIRECTIONAL-LIGHT
struct directional_light {
	vec4 ambient_intensity;
	vec4 light_colour;
	vec3 light_dir;
};
#endif

//define material
#ifndef MATERIAL
#define MATERIAL
struct material {
	vec4 emissive;
	vec4 diffuse_reflection;
	vec4 specular_reflection;
	float shininess;
}; 
#endif

//calculate directional light
vec4 calculate_direction(in directional_light light, in material mat, in vec3 transformed_normal, in vec3 view_dir, in vec4 tex_colour, in sampler1D shade)
{
	//calculate ambient light
	vec4 ambient = mat.diffuse_reflection * light.ambient_intensity;
	//calculate shade texture coords for diffuse
	float shade_tex_coord = clamp(max(dot(transformed_normal, light.light_dir), 0.0),0.0, 1.0);
	//calculate diffuse
	vec4 diffuse = texture(shade, shade_tex_coord) * (mat.diffuse_reflection * light.light_colour);
	//calculate half vector
	vec3 half_v = normalize(light.light_dir + view_dir);
	//calculate shade texture coords for specular
	shade_tex_coord = clamp(pow(max(dot(transformed_normal, half_v), 0.0),mat.shininess), 0.0, 1.0);
	//calculate specular
	vec4 specular =  texture(shade, shade_tex_coord) * (mat.specular_reflection * light.light_colour);
	//caluclate colour
	vec4 colour =  ((mat.emissive + ambient + diffuse) * tex_colour) + specular;
	//insure colour alpha is 1
	colour.a = 1.0;

	return colour;
}