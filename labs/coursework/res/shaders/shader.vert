#version 440

// Model transformation matrix
uniform mat4 M;
// transformation matrix
uniform mat4 MVP;
// normal matrix
uniform mat3 N;
// light transformation matrix
uniform mat4 lightMVP;

// incoming position
layout(location = 0) in vec3 position;
// incoming normal
layout(location = 2) in vec3 normal;
// incoming binormal
layout(location = 3) in vec3 binormal;
// incoming tangent
layout(location = 4) in vec3 tangent;
// incoming texture coordinate
layout(location = 10) in vec2 tex_coord_in;

// outgoing position
layout(location = 0) out vec3 vertex_pos;
// outgoing transformed normal
layout(location = 1) out vec2 tex_coord;
// outgoing texture coordinate
layout(location = 2) out vec3 transformed_normal;
// outgoing position in light space
layout(location = 3) out vec4 light_space_pos;
// outgoing tangent
layout(location = 4) out vec3 tangent_out;
// outgoing binormal
layout(location = 5) out vec3 binormal_out;

void main() {
  // calculate screen position
  gl_Position = MVP * vec4(position, 1.0);
  // output other values to fragment shader
  tex_coord = tex_coord_in;
  transformed_normal = N * normal;
  vertex_pos = vec3(M * vec4(position, 1.0f));
  light_space_pos = lightMVP * vec4(position, 1.0f);

  tangent_out = N * tangent;
  binormal_out = N * binormal;
}