#version 430 core

// Incoming texture containing frame information
uniform sampler2D tex;
uniform vec3 filter_col;

// Our colour filter - calculates colour intensity
const vec3 intensity = vec3(0.299, 0.587, 0.184);

// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  // *********************************
  // Sample texture colour
  colour = texture(tex, tex_coord);
  // Calculate grey value
  float i = dot(intensity, vec3(colour));
  // Use greyscale to as final colour
  // - ensure alpha is 1
  colour.rgb = vec3(i) + filter_col;
  colour.a = 1.0;
  // *********************************
}