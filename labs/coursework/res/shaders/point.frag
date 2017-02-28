#version 440


//define point light
#ifndef POINT_LIGHT
#define POINT_LIGHT
struct point_light
{
	vec4 light_colour;
	vec3 position;
	float constant;
	float linear;
	float quadratic;
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

vec4 calculate_point(in point_light point, in material mat, in vec3 vertex_pos, in vec3 transformed_normal, in vec3 view_dir, in vec4 tex_colour, in sampler1D shade)
{
	//get distance between point light and vertex
	float d = distance(point.position, vertex_pos);
	//calculate attenuation factor
	float att_fat = point.constant + point.linear * d + point.quadratic * pow(d, 2);
	//calculate shade_tex_coords
	float shade_tex_coord = clamp((1/att_fat), 0.0, 1.0);
	vec4 light_col = texture(shade, shade_tex_coord) * point.light_colour;
	//set colour alpha to 1.0
	light_col.a = 1.0f;
	//calculate light direction
	vec3 light_dir = normalize(point.position - vertex_pos);

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