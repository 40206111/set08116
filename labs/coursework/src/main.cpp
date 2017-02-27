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
map<string, texture> tex;
texture shade;

bool initialise() {
	//hide cursor
	glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//capture initial mouse position
	glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);

	return true;
}

bool load_content() {

	//create floating island 
	meshes["floating island"] = mesh(geometry("models/floating island.obj"));
//create castle
meshes["castle"] = mesh(geometry("models/castle.obj"));

//transform castle
meshes["castle"].get_transform().scale = vec3(0.3);
meshes["castle"].get_transform().translate(vec3(-1.0f, 5.8f, 0.0f));
meshes["castle"].get_transform().rotate(vec3(0.0f, half_pi<float>(), 0.0f));

// Load in shaders
eff.add_shader("shaders/shader.vert", GL_VERTEX_SHADER);
eff.add_shader("shaders/shader.frag", GL_FRAGMENT_SHADER);
// Build effect 
eff.build();

//set floating island texture
tex["floating island"] = texture("textures/isllandUV.png");
//set castle texture
tex["castle"] = texture("textures/castle.png");

//set shade data
vector<vec4> shade_data{ vec4(0.25f,0.25f,0.25f,1.0f), vec4(0.5f,0.5f,0.5f,1.0f), vec4(0.75f,0.75f, 0.75f, 1.0f), vec4(1.0f) };
//set shade texture
shade = texture(shade_data, 4, 1, false, false);

// Set camera properties
cam.set_position(vec3(0.0f, 0.0f, 30.0f));
cam.set_target(vec3(0.0f, 0.0f, 0.0f));
cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
return true;
}

void FreeCam(float delta_time)
{
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
	float speed = 10.0f * delta_time;

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
}

bool update(float delta_time) {
	static float i = 1;

	if (meshes["floating island"].get_transform().position.y>= 1 || meshes["floating island"].get_transform().position.y < 0)
	{
		i *= -1;
	}
	meshes["floating island"].get_transform().rotate(vec3(0.0f, delta_time, 0.0f));
	meshes["floating island"].get_transform().translate(vec3(0.0f, i * delta_time, 0.0f));
	cout << (meshes["floating island"].get_transform().position.y) << endl;

	FreeCam(delta_time);

	return true;
}

bool render() {
	// Bind effect
	renderer::bind(eff);

	//loop through meshes
	for (auto e : meshes)
	{
		mesh m = e.second;
		//create MVP matrix
		mat4 M = m.get_transform().get_transform_matrix();
		auto V = cam.get_view();
		auto P = cam.get_projection();
		auto MVP = P*V*M;
		//set MVP matrix uniform 
		glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

		if (e.first == "castle")
		{
			auto VP = P * V;
			M = meshes["floating island"].get_transform().get_transform_matrix() * M;
			glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(VP * M));
		}

		renderer::bind(tex[e.first], 0);
		renderer::bind(shade, 1);

		glUniform1i(eff.get_uniform_location("tex"), 0);
		glUniform1i(eff.get_uniform_location("shade_tex"), 1);
		glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));

		renderer::render(m);
	}
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