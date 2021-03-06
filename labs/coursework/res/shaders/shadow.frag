#version 440

// calculate shadow factor of a vertex
float calculate_shadow(in sampler2D shadow_map, in vec4 light_space_pos) {
  // get light screen space coordinate
  vec3 proj_coords = light_space_pos.xyz / light_space_pos.w;
  // calculate shadow texture coords
  vec2 shadow_tex_coords;
  shadow_tex_coords.x = 0.5 * proj_coords.x + 0.5;
  shadow_tex_coords.y = 0.5 * proj_coords.y + 0.5;

  // check shadow coords are in range
  if (shadow_tex_coords.x < 0 || shadow_tex_coords.x > 1 ||
      shadow_tex_coords.y < 0 || shadow_tex_coords.y > 1 || light_space_pos.z < 0) {
    return 1.0;
  }
  float z = light_space_pos.z / light_space_pos.w;

  // sample shadow map
  float depth = texture(shadow_map, shadow_tex_coords).x;

  // check if depth is in range
  if (depth == 0.0 || depth == 1.0) {
    return 1.0;
  } else if (depth < z + 0.001) {
    return 0.5;
  } else {
    return 1.0;
  }
}