#version 440

// The model matrix
uniform mat4 M;
// The transformation matrix
uniform mat4 MVP;
// The normal matrix
uniform mat3 N;
// Material colour
uniform vec4 material_colour;
// Shininess of the object
uniform float shininess;
// Light colour
uniform vec4 light_colour;
// Direction of the light
uniform vec3 light_dir;
// Position of the camera
uniform vec3 eye_pos;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 2) in vec3 normal;

// Outgoing vertex colour
layout(location = 0) out vec4 vertex_colour;

void main() {
  // *********************************
  // Calculate position
  gl_Position = MVP * vec4(position, 1.0f);
  // Transform the normal
  vec3 transformed_normal = N * normal;
  // Calculate world position
  vec4 p = M * vec4(position, 1.0f);
  // Calculate view direction
  vec4 E = (vec4(eye_pos, 1.0f) - p)/(normalize(vec4(eye_pos, 1.0f) - p));
  // Calculate half vector between view_dir and light_dir
  vec4 H = (vec4(light_dir, 1.0f) + E)/(normalize(vec4(light_dir, 1.0f) + E));
  // Calculate k
  float k = max(dot(vec4(transformed_normal, 1.0f), H), 0.0f);
  // Calculate specular
  float specular = light_colour * max(dot(vec4(N,1.0f), H), 0.0f)^shininess;
  // Ensure alpha is 1
  specular.a = 1.0f;
  // Output vertex colour - just diffuse
  vertex_colour = material_colour * specular;
  // *********************************
}