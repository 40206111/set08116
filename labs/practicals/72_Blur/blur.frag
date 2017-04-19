#version 430 core

// Incoming frame data
uniform sampler2D tex;
//Incoming mask data
uniform sampler2D mask;

// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;
// Outgoing colour
layout(location = 0) out vec4 colour;
void main() {

vec4 col1 = texture(tex, tex_coord);
vec4 col2 = texture(mask, tex_coord);

  colour = col1 * col2;
  colour.a = 1.0;
  // *********************************
}