#version 430 core

// Incoming frame data
uniform sampler2D tex;
// 1.0f / screen width
uniform float inverse_width;
// 1.0f / screen height
uniform float inverse_height;
// Surrounding pixels to sample and their scale
const vec4 samples[5] = vec4[5](vec4(-1.0, 0.0, 0.0, -2.0/3.0), vec4(0.0, 1.0, 0.0, -2.0/3.0), vec4(1.0, 0.0, 0.0, -2.0/3.0),
                                vec4(0.0, -1.0, 0.0, -2.0/3.0), vec4(0.0, 0.0, 0.0, 11.0/3.0));
// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;
// Outgoing colour
layout(location = 0) out vec4 colour;
void main() {
    // *********************************
  // Start with colour as black
  colour = vec4(vec3(0.0), 1.0);
    // Loop through each sample vector
  for (int i = 0; i < 5; ++i)
  {
        // Calculate tex coord to sample
		vec2 uv = tex_coord + vec2(samples[i].x * inverse_width, samples[i].y * inverse_height);
    // Sample the texture and scale appropriately
    // - scale factor stored in w component
	colour += texture(tex, uv) * samples[i].w;
}
  // Ensure alpha is 1.0
  colour.a = 1.0;
  // *********************************
}