#version 440

// Spot light data
struct spot_light {
  vec4 light_colour;
  vec3 position;
  vec3 direction;
  float constant;
  float linear;
  float quadratic;
  float power;
};

// Material data
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// Spot light being used in the scene
uniform spot_light spot;
// Material of the object being rendered
uniform material mat;
// Position of the eye (camera)
uniform vec3 eye_pos;
// Texture to sample from
uniform sampler2D tex;

// Incoming position
layout(location = 0) in vec3 vertex_pos;
// Incoming normal
layout(location = 1) in vec3 transformed_normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  // *********************************
  // Calculate direction to the light
  vec3 light_dir = normalize(spot.position - vertex_pos);
  // Calculate distance to light
  float d = distance(spot.position, vertex_pos);
  // Calculate attenuation value
   float att_fat = spot.constant + spot.linear * d + spot.quadratic* pow(d, 2);
  // Calculate spot light intensity
  float intensity = max(dot(-light_dir, spot.direction), 0.0f);
  // Calculate light colour
  vec4 light_col = ((pow(intensity, spot.power)) /att_fat) * spot.light_colour;
  // Calculate view direction
    vec3 view_dir = normalize(eye_pos - vertex_pos);
  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
  vec4 diffuse = max(dot(transformed_normal, light_dir), 0.0f) * (mat.diffuse_reflection * light_col);
  vec3 half_v = normalize(light_dir + view_dir);
  vec4 specular = pow(max(dot(transformed_normal, half_v), 0.0f), mat.shininess) * (mat.specular_reflection * light_col);
  vec4 tex_colour = texture(tex, tex_coord);
  vec4 primary = mat.emissive + diffuse;
  colour = primary * tex_colour + specular;
  colour.a = 1.0f;
  // *********************************
}