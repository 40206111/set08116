#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

effect eff;
texture tex;
mesh m;
target_camera cam;
directional_light light;

bool load_content() {
  // *********************************
  // Load in model, models/teapot.obj
	m = mesh(geometry("models/floating island.obj"));
  // Load in texture, textures/checker.png
	tex = texture("textures/isllandUV.png");
  // *********************************
	//m.get_transform().scale = vec3(50);

	// ambient intensity (0.3, 0.3, 0.3)
	light.set_ambient_intensity(vec4(0.3f, 0.3f, 0.3f, 1.0f));
	// Light colour white
	light.set_light_colour(vec4(1.0f));
	// Light direction (1.0, 1.0, -1.0)
	light.set_direction(vec3(1.0f, 1.0f, -1.0f));
  // Load in shaders
  eff.add_shader("48_Phong_Shading/phong.vert", GL_VERTEX_SHADER);
  eff.add_shader("48_Phong_Shading/phong.frag", GL_FRAGMENT_SHADER);
  // Build effect
  eff.build();

  // Set camera properties
  cam.set_position(vec3(0.0f, 0.0f, 10.0f));
  cam.set_target(m.get_transform().position);

  cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
  return true;
}

bool update(float delta_time) {

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_A)) {
		m.get_transform().rotate(vec3(0.0f, -pi<float>(), 0.0f) * delta_time);
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_D)) {
		m.get_transform().rotate(vec3(0.0f, pi<float>(), 0.0f) * delta_time);
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_W)) {
		m.get_transform().rotate(vec3(-pi<float>(), 0.0f, 0.0f) * delta_time);
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_S)) {
		m.get_transform().rotate(vec3(pi<float>(), 0.0f, 0.0f) * delta_time);
	}
  // Update the camera
  cam.update(delta_time);
  return true;
}

bool render() {
  // Bind effect
  renderer::bind(eff);
  // Create MVP matrix
  auto M = m.get_transform().get_transform_matrix();
  auto V = cam.get_view();
  auto P = cam.get_projection();
  auto MVP = P * V * M;
  // Set MVP matrix uniform
  glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));


  glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));

  // **************************************
  // Set N matrix uniform - remember - 3x3 matrix
  glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));
  // Bind material
  renderer::bind(m.get_material(), "mat");
  // Bind light
  renderer::bind(light, "light");
  // Bind texture
  renderer::bind(tex, 0);
  // Set tex uniform
  glUniform1i(eff.get_uniform_location("tex"), 0);
  // Set eye position - Get this from active camera
  glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(cam.get_position()));

  // Render mesh
  renderer::render(m);

  return true;
}

void main() {
  // Create application
  app application("36_Loading_Models");
  // Set load content, update and render methods
  application.set_load_content(load_content);
  application.set_update(update);
  application.set_render(render);
  // Run application
  application.run();
}