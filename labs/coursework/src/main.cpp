#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

geometry geom;
effect eff;
free_camera cam;
map<string, mesh> meshes;
double cursor_x = 0;
double cursor_y = 0;
texture tex;

bool initialise() {
	//hide cursor
	glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//capture initial mouse position
	glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);

	return true;
}

bool load_content() {
	/*
  // Create triangle data
  vector<vec3> positions{vec3(0.0f, 1.0f, 0.0f), vec3(-1.0f, -1.0f, 0.0f), vec3(1.0f, -1.0f, 0.0f)

  };
  // Colours
  vector<vec4> colours{vec4(1.0f, 0.0f, 0.0f, 1.0f), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec4(1.0f, 0.0f, 0.0f, 1.0f)};
  // Add to the geometry
  geom.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
  geom.add_buffer(colours, BUFFER_INDEXES::COLOUR_BUFFER);
  */

  //create floating island
	meshes["floating island"] = mesh(geometry("models/floating island.obj"));

	// Load in shaders
	eff.add_shader("shaders/shader.vert", GL_VERTEX_SHADER);
	eff.add_shader("shaders/shader.frag", GL_FRAGMENT_SHADER);
	// Build effect
	eff.build();

	tex = texture("textures/isllandUV.png");

	// Set camera properties
	cam.set_position(vec3(0.0f, 0.0f, 10.0f));
	cam.set_target(vec3(0.0f, 0.0f, 0.0f));
	cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
	return true;
}


bool update(float delta_time) {

	//ratio of pixels to rotation
	static double ratio_width = quarter_pi<float>() / static_cast<float>(renderer::get_screen_width());
	static double ratio_height = (quarter_pi<float>()
		* (static_cast<float>(renderer::get_screen_height()) / static_cast<float>(renderer::get_screen_width())))
		/ static_cast<float>(renderer::get_screen_height());

	//variables for mouse position
	double current_x = 0;
	double current_y = 0;

	//get current cursor position
	glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);

	//get current cursor position
	double delta_x = current_x - cursor_x;
	double delta_y = current_y - cursor_y;

	//multiply deltas by ratios
	delta_x *= ratio_width;
	delta_y *= ratio_height;

	//rotate cam
	cam.rotate(delta_x, -delta_y);

	//set speed
	float speed = 5.0f * delta_time;

	//use keyboard to move camera - WASD
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_W))
	{
		cam.move(vec3(0.0f, 0.0f, speed));
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_S))
	{
		cam.move(vec3(0.0f, 0.0f, -speed));
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_A))
	{
		cam.move(vec3(-speed, 0.0f, 0.0f));
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_D))
	{
		cam.move(vec3(speed, 0.0f, 0.0f));
	}
	//move camera up and down - space, shift
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_SPACE))
	{
		cam.move(vec3(0.0f, speed, 0.0f));
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT_SHIFT))
	{
		cam.move(vec3(0.0f, -speed, 0.0f));
	}

	// Update the camera
	cam.update(delta_time);

	//update cursot position
	cursor_x = current_x;
	cursor_y = current_y;

	return true;
}

bool render() {
	// Bind effect
	renderer::bind(eff);
	/*
	// Create MVP matrix
	mat4 M(1.0f);
	auto V = cam.get_view();
	auto P = cam.get_projection();
	auto MVP = P * V * M;
	// Set MVP matrix uniform
	glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	// Render geometry
	renderer::render(geom);*/
	mesh m = meshes["floating island"];
	//create MVP matrix
	mat4 M = m.get_transform().get_transform_matrix();
	auto V = cam.get_view();
	auto P = cam.get_projection();
	auto MVP = P*V*M;
	//set MVP matrix uniform
	glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

	renderer::render(m);

	return true;
}

void main() {
	// Create application
	app application("Graphics Coursework");
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_initialise(initialise);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}