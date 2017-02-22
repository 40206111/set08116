// Point light information
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

// Point light calculation
vec4 calculate_point(in point_light point, in material mat, in vec3 vertex_pos, in vec3 transformed_normal, in vec3 view_dir, in vec4 tex_colour)
{
	// *********************************
	// Get distance between point light and vertex
	float d = distance(point.position, vertex_pos);
	// Calculate attenuation factor : constant + (linear * d) + (quadratic * d * d)
	float att_fat = point.constant + point.linear * d + point.quadratic * pow(d, 2);
	// Calculate light colour : light_colour / attenuation
	vec4 light_col = (1/att_fat) * point.light_colour;
	//Set colour alpha to 1.0
	light_col.a = 1.0f;
	// Calculate light dir
	vec3 light_dir = normalize(point.position - vertex_pos);
	// *********************************
	// Now use standard phong shading but using calculated light colour and direction
	// - note no ambient
	vec4 diffuse = (mat.diffuse_reflection * light_col) * max(dot(transformed_normal, light_dir), 0);
	vec3 half_vector = normalize(light_dir + view_dir);
	vec4 specular = (mat.specular_reflection * light_col) * pow(max(dot(transformed_normal, half_vector), 0), mat.shininess);
	vec4 primary = mat.emissive + diffuse;
	vec4 colour = primary * tex_colour + specular;
	colour.a = 1.0;
	return colour;
}