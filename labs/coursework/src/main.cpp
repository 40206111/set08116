#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

effect eff;
effect shadow_eff;
effect sky_eff;
effect simple;
effect edges;
effect masking;
effect norms;
effect silh;
effect wires;
frame_buffer frame;
frame_buffer mask;
free_camera cam;
arc_ball_camera ABcam;
mesh skybox = skybox;
map<string, mesh> meshes;
geometry screen_quad;
double cursor_x = 0;
double cursor_y = 0;
map<string, texture> tex;
texture shade;
map<string, texture> normal_map;
cubemap cube_map;
directional_light light;
point_light point;
vector<spot_light> spot(3);
shadow_map shadow;
int use_cam = 0;
bool shadow_on = false;
bool normals_on = false;
bool wireframe_on = false;
bool atmptLines_on = false;
mat4 VP;
vec3 filter_col(0.0, 0.0, 0.0);

bool initialise() {
	// hide cursor
	glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// capture initial mouse position
	glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);

	return true;
}

bool load_content() {
	//initialise frames
	frame = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());
	mask = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());

	//screen quad positions
	vector<vec3> positions{ vec3(-1.0f, -1.0f, 0.0f), vec3(1.0f, -1.0f, 0.0f), vec3(-1.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f) };
	vector<vec2> tex_coords{ vec2(0.0, 0.0), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f) };
	screen_quad.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
	screen_quad.add_buffer(tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);
	screen_quad.set_type(GL_TRIANGLE_STRIP);

	// create shadow map
	shadow = shadow_map(renderer::get_screen_width(), renderer::get_screen_height());
	// create plane for viewing shadows
	meshes["plane"] = mesh(geometry_builder::create_plane());

	// create skybox
	skybox = mesh(geometry_builder::create_box());

	// create floating islands
	meshes["floating island"] = mesh(geometry("models/floating island.obj"));
	meshes["island2"] = mesh(geometry("models/island2.obj"));
	meshes["island3"] = mesh(geometry("models/island3.obj"));

	//translate plane
	meshes["plane"].get_transform().translate(vec3(0.0f, -25.0f, 0.0f));
	// create castle
	meshes["castle"] = mesh(geometry("models/castle.obj"));

	// scale skybox
	skybox.get_transform().scale = vec3(100.0f);
	// translate island2
	meshes["island2"].get_transform().translate(vec3(50.0f, 0.0f, -100.0f));
	// translate island3
	meshes["island3"].get_transform().translate(vec3(-50.0f, 0.0f, -90.0f));
	// transform castle
	meshes["castle"].get_transform().scale = vec3(0.3);
	meshes["castle"].get_transform().translate(vec3(-1.0f, 5.3f, 0.0f));
	meshes["castle"].get_transform().rotate(vec3(0.0f, half_pi<float>(), 0.0f));

	// Load in shaders
	eff.add_shader("shaders/shader.vert", GL_VERTEX_SHADER);
	eff.add_shader("shaders/shader.frag", GL_FRAGMENT_SHADER);
	eff.add_shader("shaders/directional.frag", GL_FRAGMENT_SHADER);
	eff.add_shader("shaders/spot.frag", GL_FRAGMENT_SHADER);
	eff.add_shader("shaders/point.frag", GL_FRAGMENT_SHADER);
	eff.add_shader("shaders/shadow.frag", GL_FRAGMENT_SHADER);
	eff.add_shader("shaders/normal_map.frag", GL_FRAGMENT_SHADER);
	// Build effect
	eff.build();

	// load shadow vert
	shadow_eff.add_shader("shaders/shadow.vert", GL_VERTEX_SHADER);
	// build shadow effect
	shadow_eff.build();

	// load skybox shaders
	sky_eff.add_shader("shaders/skybox.vert", GL_VERTEX_SHADER);
	sky_eff.add_shader("shaders/skybox.frag", GL_FRAGMENT_SHADER);
	// build skybox effect
	sky_eff.build();

	//load simple shaders
	simple.add_shader("shaders/simple.vert", GL_VERTEX_SHADER);
	simple.add_shader("shaders/simple.frag", GL_FRAGMENT_SHADER);
	//build simple effect
	simple.build();

	//load edges shaders
	edges.add_shader("shaders/simple.vert", GL_VERTEX_SHADER);
	edges.add_shader("shaders/edges.frag", GL_FRAGMENT_SHADER);
	//build edges effect
	edges.build();

	//load masking shaders
	masking.add_shader("shaders/simple.vert", GL_VERTEX_SHADER);
	masking.add_shader("shaders/masking.frag", GL_FRAGMENT_SHADER);
	//build masking effect#
	masking.build();

	//load normals shaders
	norms.add_shader("shaders/simpler.vert", GL_VERTEX_SHADER);
	norms.add_shader("shaders/simpler.frag", GL_FRAGMENT_SHADER);
	norms.add_shader("shaders/normals.geom", GL_GEOMETRY_SHADER);
	//build normals shader
	norms.build();

	//load wireframe shaders
	wires.add_shader("shaders/simpler.vert", GL_VERTEX_SHADER);
	wires.add_shader("shaders/simpler.frag", GL_FRAGMENT_SHADER);
	wires.add_shader("shaders/wireframe.geom", GL_GEOMETRY_SHADER);
	//build silhouette shader
	wires.build();

	//load silhouette shaders
	silh.add_shader("shaders/simpler.vert", GL_VERTEX_SHADER);
	silh.add_shader("shaders/simpler.frag", GL_FRAGMENT_SHADER);
	silh.add_shader("shaders/silhouette.geom", GL_GEOMETRY_SHADER);
	//build silhouette shader
	silh.build();

	//set plane texture
	tex["plane"] = texture("textures/blank_normal.png");
	// set floating island texture
	tex["floating island"] = texture("textures/isllandUV.png");
	// set castle texture
	tex["castle"] = texture("textures/castle.png");
	// set island2 texture
	tex["island2"] = texture("textures/island2.png");
	// set island3 texture
	tex["island3"] = texture("textures/island3.png");

	// set normal maps
	normal_map["bricks"] = texture("textures/castle_normal.png");
	normal_map["blank"] = texture("textures/blank_normal.png");

	// skybox filenames
	array<string, 6> filenames = { "textures/front2.png", "textures/back2.png",
								  "textures/up2.png",    "textures/down2.png",
								  "textures/right2.png", "textures/left2.png" };
	// set cube map
	cube_map = cubemap(filenames);

	// set light
	light.set_ambient_intensity(vec4(0.1f));
	light.set_light_colour(vec4(1.0f));
	light.set_direction(normalize(vec3(-1.0, 0.5, 0.4)));
	spot[0].set_position(vec3(0.0f, 25.0f, 0.0f));
	spot[1].set_position(vec3(50.0f, 30.0f, -101.0f));
	spot[2].set_position(vec3(-51.0f, 22.0f, -89.0f));

	// set all spotlight values
	for (int i = 0; i < 3; i++) {
		// set spot light
		spot[i].set_light_colour(vec4(0.0f, 0.0f, 0.0f, 1.0f));
		spot[i].set_direction(normalize(vec3(0.0f, -1.0f, 0.0f)));
		spot[i].set_range(20.0f);
		spot[i].set_power(10.0f);
		spot[i].set_constant_attenuation(0.1f);
	}

	// set point light
	point.set_light_colour(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	point.set_position(vec3(-6.0f, 3.0f, 0.0f));
	point.set_range(5.0f);

	material mat;
	// set default material
	mat.set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	mat.set_diffuse(vec4(1.0f));
	mat.set_shininess(50.0f);
	mat.set_specular(vec4(0.0f, 0.0f, 0.0f, 1.0f));

	// set meshes specific material
	meshes["floating island"].set_material(mat);
	meshes["island2"].set_material(mat);
	meshes["island3"].set_material(mat);
	meshes["castle"].set_material(mat);
	//set specular and diffuse of plane
	mat.set_specular(vec4(0.0f, 0.8f, 0.2f, 1.0f));
	mat.set_diffuse(vec4(1.0f, 0.0f, 0.5f, 1.0f));
	meshes["plane"].set_material(mat);

	// set shade data
	vector<vec4> shade_data{vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(0.25f, 0.25f, 0.25f, 1.0f), vec4(0.5f, 0.5f, 0.5f, 1.0f), vec4(0.75f, 0.75f, 0.75f, 1.0f), vec4(1.0f)};
	// set shade texture
	shade = texture(shade_data, 5, 1, false, false);

	// Set free camera properties
	cam.set_position(vec3(3.0f, 5.0f, 30.0f));
	cam.set_target(vec3(0.0f, 0.0f, 0.0f));
	cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);

	// set arc ball camera properties
	ABcam.set_target(meshes["floating island"].get_transform().position);
	ABcam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f,
		1000.0f);
	ABcam.move(20.0f);

	return true;
}

// free camera method
void FreeCam(float delta_time) {

	// reset camera position
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_C)) {
		cam.set_position(vec3(3.0f, 5.0f, 30.0f));
		cam.set_pitch(0.0f);
		cam.set_yaw(0.0f);
	}
	// reset camera orientation
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_X)) {
		cam.set_pitch(0.0f);
		cam.set_yaw(0.0f);
	}

	// ratio of pixels to rotation
	static double ratio_width = quarter_pi<float>() / static_cast<float>(renderer::get_screen_width());
	static double ratio_height = (quarter_pi<float>() * (static_cast<float>(renderer::get_screen_height()) / static_cast<float>(renderer::get_screen_width()))) / static_cast<float>(renderer::get_screen_height());

	// variables for mouse position
	double current_x = 0;
	double current_y = 0;

	// get current cursor position
	glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);

	// get current cursor position
	double delta_x = current_x - cursor_x;
	double delta_y = current_y - cursor_y;

	// multiply deltas by ratios
	delta_x *= ratio_width;
	delta_y *= ratio_height;

	//rotate camera
	cam.rotate(delta_x, -delta_y);

	// update cursor position
	cursor_x = current_x;
	cursor_y = current_y;

	// set speed
	float speed = 10.0f * delta_time;
	// set speed for up and down movement
	float hspeed = 5.0f * delta_time;

	// speed up
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT_CONTROL)) {
		speed = 100.0f * delta_time;
		hspeed = 50.0f * delta_time;
	}
	// use keyboard to move camera - WASD
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_W)) {
		cam.move(vec3(0.0f, 0.0f, speed));
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_S)) {
		cam.move(vec3(0.0f, 0.0f, -speed));
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_A)) {
		cam.move(vec3(-speed, 0.0f, 0.0f));
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_D)) {
		cam.move(vec3(speed, 0.0f, 0.0f));
	}
	// move camera up and down - space, shift
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_SPACE)) {
		cam.move(vec3(0.0f, hspeed, 0.0f));
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT_SHIFT)) {
		cam.move(vec3(0.0f, -hspeed, 0.0f));
	}
	// Update the camera
	cam.update(delta_time);

	// set skybox position to camera position
	skybox.get_transform().position = cam.get_position();
}

// method to move arc ball camera with scroll wheel
void scroll(GLFWwindow *window, double x, double y) {
	// change camera distance - scroll wheel
	if (y > 0) {
		ABcam.move(y * -1.0f);
	}
	if (y < 0) {
		ABcam.move(y * -1.0f);
	}
}

// arc ball camera method
void Arc_ball_cam(float delta_time) {
	// meshes arc ball cam can target
	static std::array<mesh, 3> targets{ meshes["floating island"], meshes["island2"], meshes["island3"] };
	// int to scroll through meshes with
	static int i = 0;
	static bool down = false;

	// switch to next island when N is pressed
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_N) == GLFW_PRESS && !down) {
		// set up to false and down to true
		down = true;
		// increment i
		i++;
		// reset i to 0
		if (i >= 3) {
			i = 0;
		}
		// set arc ball camera target
		ABcam.set_target(targets[i].get_transform().position);
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_N) == GLFW_RELEASE && down) {
		down = false;
	}

	// ratio of pixels to rotation
	static double ratio_width =
		quarter_pi<float>() / static_cast<float>(renderer::get_screen_width());
	static double ratio_height =
		(quarter_pi<float>() *
		(static_cast<float>(renderer::get_screen_height()) /
			static_cast<float>(renderer::get_screen_width()))) /
		static_cast<float>(renderer::get_screen_height());

	// variables for mouse position
	double current_x = 0;
	double current_y = 0;

	// get current cursor position
	glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);

	// get current cursor position
	double delta_x = current_x - cursor_x;
	double delta_y = current_y - cursor_y;

	// multiply deltas by ratios
	delta_x *= ratio_width;
	delta_y *= ratio_height;

	// rotate camera
	ABcam.rotate(-delta_y, delta_x);

	// Update cursor pos
	cursor_x = current_x;
	cursor_y = current_y;


	glfwSetScrollCallback(renderer::get_window(), scroll);
	// update camera
	ABcam.update(delta_time);

	// set skybox to arc ball camera position
	skybox.get_transform().position = ABcam.get_position();
}

// method to control scene lights
void LightControl() {
	// bools to store on states of lights
	static bool point_on = false;
	static bool spot_on = false;
	static bool dir_on = true;
	// bools to test if button is pressed or not
	static bool pdown = false;
	static bool odown = false;
	static bool ldown = false;
	static bool kdown = false;

	//toggle point light////////////////////////////////
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_P) == GLFW_RELEASE &&
		pdown) {
		pdown = false;

		point_on = !point_on;

		// set point light colour
		if (point_on) {
			point.set_light_colour(vec4(1.0f));
		}
		if (!point_on) {
			point.set_light_colour(vec4(0.0f, 0.0f, 0.0f, 1.0f));
		}

	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_P) == GLFW_PRESS && !pdown) {
		pdown = true;
	}
	/////////////////Point light toggle end//

	//toggle shadow///////////////////////////////////
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_K) == GLFW_RELEASE && spot_on && kdown)
	{
		kdown = false;

		// toggle shadow
		shadow_on = !shadow_on;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_K) == GLFW_PRESS && spot_on && !kdown) {
		kdown = true;
	}
	/////////////////Shadow toggle end//

	//toggle spot light////////////////////////////////
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_O) == GLFW_RELEASE &&
		odown) {
		odown = false;

		spot_on = !spot_on;

		// set spot light colours
		if (spot_on) {
			for (int i = 0; i < 3; i++) {
				spot[i].set_light_colour(vec4(1.0f));
			}
		}
		if (!spot_on) {
			for (int i = 0; i < 3; i++) {
				spot[i].set_light_colour(vec4(0.0f, 0.0f, 0.0f, 1.0f));
				shadow_on = false;
			}
		}

	}

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_O) == GLFW_PRESS && !odown) {
		odown = true;
	}
	/////////////////Spot light toggle end//

	//toggle directional light///////////////////////////
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_L) == GLFW_RELEASE &&
		ldown) {
		ldown = false;

		dir_on = !dir_on;

		// set directional light colours
		if (dir_on) {
			light.set_light_colour(vec4(1.0f));
		}
		if (!dir_on) {
			light.set_light_colour(vec4(0.0f, 0.0f, 0.0f, 1.0f));
		}

	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_L) == GLFW_PRESS && !ldown) {
		ldown = true;
	}
	/////////////////directional light toggle end//
}

//method for colouring filter
void filterControl(float delta_time)
{
	//increase red -R
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_R) && !glfwGetKey(renderer::get_window(), GLFW_KEY_RIGHT_SHIFT) && filter_col.r != 1.0f)
	{
		filter_col += vec3(0.1f * delta_time, 0.0f, 0.0f);
	}
	//decrease red -Right Shift + R
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_R) && glfwGetKey(renderer::get_window(), GLFW_KEY_RIGHT_SHIFT) && filter_col.r != 0.0f)
	{
		filter_col -= vec3(0.1f * delta_time, 0.0f, 0.0f);
	}

	//increase green -G
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_G) && !glfwGetKey(renderer::get_window(), GLFW_KEY_RIGHT_SHIFT) && filter_col.g != 1.0f)
	{
		filter_col += vec3(0.0f, 0.1f * delta_time, 0.0f);
	}
	//decrease green -Right Shift + G
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_G) && glfwGetKey(renderer::get_window(), GLFW_KEY_RIGHT_SHIFT) && filter_col.g != 0.0f)
	{
		filter_col -= vec3(0.0f, 0.1f * delta_time, 0.0f);
	}

	//increase blue -B
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_B) && !glfwGetKey(renderer::get_window(), GLFW_KEY_RIGHT_SHIFT) && filter_col.b != 1.0f)
	{
		filter_col += vec3(0.0f, 0.0f, 0.1f * delta_time);
	}
	//decrease blue -Right Shift + B
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_B) && glfwGetKey(renderer::get_window(), GLFW_KEY_RIGHT_SHIFT) && filter_col.b != 0.0f)
	{
		filter_col -= vec3(0.0f, 0.0f, 0.1f * delta_time);
	}
}

//method for toggling debug modes
void debugControl()
{
	static bool ndown = false;
	static bool mdown = false;
	static bool hashdown = false;

	//NORMALS TOGGLE
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_COMMA) == GLFW_RELEASE &&
		ndown) {
		ndown = false;
		//turn normals on
		normals_on = !normals_on;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_COMMA) == GLFW_PRESS && !ndown) {
		ndown = true;
	}
	//NORMALS TOGGLE END

	//WIREFRAME TOGGLE
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_PERIOD) == GLFW_RELEASE &&
		mdown) {
		mdown = false;
		//turn wireframe on
		wireframe_on = !wireframe_on;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_PERIOD) == GLFW_PRESS && !mdown) {
		mdown = true;
	}
	//WIREFRAME TOGGLE END

	//GEOMETRY OUTLINES TOGGLE
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_GRAVE_ACCENT) == GLFW_RELEASE &&
		hashdown) {
		hashdown = false;
		//turn attempted lines on
		atmptLines_on = !atmptLines_on;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS && !hashdown) {
		hashdown = true;
	}
	//GEOMETRY OUTLINES TOGGLE END
}

//method for toggling all movement on and off
void islandControl(float delta_time)
{
	static bool sixdown = false;
	static bool pause = false;
	// calculate total time
	static float total_time = 0;
	total_time += delta_time;

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_6) == GLFW_RELEASE &&
		sixdown) {
		sixdown = false;
		pause = !pause;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_6) == GLFW_PRESS && !sixdown) {
		sixdown = true;
	}

	if (!pause)
	{
		// rotate and bob islands
		meshes["floating island"].get_transform().rotate(
			vec3(0.0f, delta_time * 0.5, 0.0f));
		meshes["floating island"].get_transform().translate(
			vec3(0.0f, sin(total_time) * 0.02, 0.0f));
		meshes["island2"].get_transform().rotate(vec3(0.0f, delta_time * 0.3f, 0.0f));
		meshes["island2"].get_transform().translate(
			vec3(0.0f, sin(total_time - 1.5f) * 0.02, 0.0f));
		meshes["island3"].get_transform().rotate(vec3(0.0f, delta_time * 0.7f, 0.0f));
		meshes["island3"].get_transform().translate(
			vec3(0.0f, sin(total_time - 2.5f) * 0.02, 0.0f));
	}
}

bool update(float delta_time) {
	//control islands
	islandControl(delta_time);

	// control light
	LightControl();

	// change current camera
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_0) && use_cam != 0) {
		use_cam = 0;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_1) && use_cam != 1) {
		use_cam = 1;
	}

	//control filter
	filterControl(delta_time);

	//toggle debug views
	debugControl();

	// use controls for current camera
	switch (use_cam) {
	case 1:
		// control Arc ball camera
		Arc_ball_cam(delta_time);
		break;
	default:
		// control camera using free cam
		FreeCam(delta_time);
	}

	//allow plane to be moved if shadow is on
	if (shadow_on) {
		// translate plane on y axis
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_UP)) {
			meshes["plane"].get_transform().translate(
				vec3(0.0f, 10 * delta_time, 0.0f));
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_DOWN)) {
			meshes["plane"].get_transform().translate(
				vec3(0.0f, -10 * delta_time, 0.0f));
		}
	}
	return true;
}

//method to render skybox
void renderSkyBox(mat4 MVP)
{
	// disable depth test, depth mask and face culling
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	// bind sky effect
	renderer::bind(sky_eff);

	// set MVP uniform
	glUniformMatrix4fv(sky_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

	// bind cubemap and set uniform
	renderer::bind(cube_map, 3);
	glUniform1i(sky_eff.get_uniform_location("cubemap"), 3);

	// render skybox
	renderer::render(skybox);

	// enable depth test, depth mask and cull face
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
}

//method to render meshes
void renderMeshes(mat4 VP)
{
	// Bind effect
	renderer::bind(eff);
	glUniform1i(eff.get_uniform_location("shade_tex"), 1);
	renderer::bind(shade, 1);
	renderer::bind(shadow.buffer->get_depth(), 5);
	// set shade uniform
	glUniform1i(eff.get_uniform_location("shade_tex"), 1);
	// set shadow map uniform
	glUniform1i(eff.get_uniform_location("shadow_map"), 5);

	switch (use_cam) {
	case 1:
		// set eye position to camera position
		glUniform3fv(eff.get_uniform_location("eye_pos"), 1,
			value_ptr(ABcam.get_position()));
		break;
	default:
		// set eye position to camera position
		glUniform3fv(eff.get_uniform_location("eye_pos"), 1,
			value_ptr(cam.get_position()));
		break;
	}

	// loop through meshes
	for (auto e : meshes) {

		// don't render plane if shadow isn't on
		if (e.first == "plane" && !shadow_on) {
			break;
		}

		mesh m = e.second;
		// create MVP matrix
		mat4 M = m.get_transform().get_transform_matrix();

		// calculate MVP
		mat4 MVP = VP * M;

		// set MVP matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		// set normal matrix
		glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));

		// bind default normal map
		renderer::bind(normal_map["blank"], 2);

		// put castle in transform hierarchy of floating island
		if (e.first == "castle") {
			M = meshes["floating island"].get_transform().get_transform_matrix() * M;
			glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE,value_ptr(VP * M));
			// set normal matrix
			glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(meshes["floating island"].get_transform().get_normal_matrix() * m.get_transform().get_normal_matrix()));
			// bind castle normal map
			renderer::bind(normal_map["bricks"], 2);
		}
		// set lights to hierarchy of island they're on
		if (e.first == "floating island") {
			vec4 light_pos = M * vec4(-6.0f, 3.0f, 0.0f, 1.0f);
			point.set_position(vec3(light_pos) / (light_pos.w));
			light_pos = M * vec4(0.0f, 25.0f, 0.0f, 1.0f);
			spot[0].set_position(vec3(light_pos) / (light_pos.w));
		}
		if (e.first == "island2") {
			vec4 light_pos = M * vec4(0.0f, 30.0f, 0.0f, 1.0f);
			spot[1].set_position(vec3(light_pos) / (light_pos.w));
		}
		if (e.first == "island3") {
			vec4 light_pos = M * vec4(0.0f, 22.0f, 0.0f, 1.0f);
			spot[2].set_position(vec3(light_pos) / (light_pos.w));
		}
		// set light perspective and view
		auto LP = perspective<float>(half_pi<float>(), renderer::get_screen_aspect(),
			0.1f, 1000.0f);
		auto LV = shadow.get_view();
		// set model matrix
		glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE,
			value_ptr(M));
		// set light MVP uniform
		glUniformMatrix4fv(eff.get_uniform_location("lightMVP"), 1, GL_FALSE,
			value_ptr(LP * LV * M));

		// bind textures
		renderer::bind(tex[e.first], 0);

		// set normal map uniform
		glUniform1i(eff.get_uniform_location("normal_map"), 2);
		// set texture uniform
		glUniform1i(eff.get_uniform_location("tex"), 0);

		// bind material
		renderer::bind(m.get_material(), "mat");

		// render mesh
		renderer::render(m);
	}
	// bind lights
	renderer::bind(light, "light");
	renderer::bind(spot, "spot");
	renderer::bind(point, "point");
}

//method to render meshes simply using given effect
void renderSimpleMeshes(mat4 VP, effect simp)
{
	// loop through meshes
	for (auto e : meshes) {

		// don't render plane if shadow isn't on
		if (e.first == "plane" && !shadow_on) {
			break;
		}

		mesh m = e.second;
		// create MVP matrix
		mat4 M = m.get_transform().get_transform_matrix();

		// calculate MVP
		mat4 MVP = VP* M;

		// set MVP matrix uniform
		glUniformMatrix4fv(simp.get_uniform_location("MVP"), 1, GL_FALSE,
			value_ptr(MVP));
		// set model matrix uniform
		glUniformMatrix4fv(simp.get_uniform_location("M"), 1, GL_FALSE,
			value_ptr(M));
		// set model matrix uniform
		glUniformMatrix3fv(simp.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));

		// put castle in transform hierarchy of floating island
		if (e.first == "castle") {
			M = meshes["floating island"].get_transform().get_transform_matrix() * M;
			// set matrix model uniform
			glUniformMatrix4fv(simp.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
			glUniformMatrix4fv(simp.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(VP * M));
		}

		// bind textures
		renderer::bind(tex[e.first], 0);
		// set texture uniform
		glUniform1i(simp.get_uniform_location("tex"), 0);

		// render mesh
		renderer::render(m);
	}
}

//method to render edges
void renderEdges()
{
	//set render target to mask frame buffer
	renderer::set_render_target(mask);
	renderer::clear();

	//bind edges effect
	renderer::bind(edges);

	//set MVP
	glUniformMatrix4fv(edges.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(mat4(1.0f)));

	//bind frame buffer frame and set uniform
	renderer::bind(frame.get_frame(), 0);
	glUniform1i(edges.get_uniform_location("tex"), 0);

	//set inverse width and height uniforms
	glUniform1f(edges.get_uniform_location("inverse_width"), 1.0f / renderer::get_screen_width());
	glUniform1f(edges.get_uniform_location("inverse_height"), 1.0f / renderer::get_screen_height());

	//render screen quad
	renderer::render(screen_quad);
}

//method to render masking
void renderMasking()
{
	//set render target to screen
	renderer::set_render_target();

	//bind masking effect
	renderer::bind(masking);

	//set MVP uniform
	glUniformMatrix4fv(masking.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(mat4(1.0f)));

	//bind frame buffer frame and mask
	renderer::bind(frame.get_frame(), 0);
	renderer::bind(mask.get_frame(), 1);

	//set texture and mask uniforms
	glUniform1i(masking.get_uniform_location("tex"), 0);
	glUniform1i(masking.get_uniform_location("mask"), 1);

	//set filter colour uniform
	glUniform3fv(masking.get_uniform_location("filter_col"), 1, value_ptr(filter_col));

	//render screen quad
	renderer::render(screen_quad);
}

//method to render shadows
void renderShadows()
{
	// set light perspective and view
	auto LP = perspective<float>(half_pi<float>(), renderer::get_screen_aspect(),
		0.1f, 1000.0f);
	auto LV = shadow.get_view();

	// bind shadow effect
	renderer::bind(shadow_eff);

	// sset render target to shadow
	renderer::set_render_target(shadow);
	// set shadow light direction and position
	shadow.light_dir = spot[0].get_direction();
	shadow.light_position = spot[0].get_position();

	// set cull face to front
	glCullFace(GL_FRONT);
	// clear depth buffer
	glClear(GL_DEPTH_BUFFER_BIT);

	// loop through meshes
	for (auto &e : meshes) {
		auto m = e.second;
		// set model matrix
		mat4 M = m.get_transform().get_transform_matrix();

		// put castle in transform hierarchy of floating island
		if (e.first == "castle") {
			M = meshes["floating island"].get_transform().get_transform_matrix() *
				M;
		}

		// set MVP uniform for shadow effect
		glUniformMatrix4fv(shadow_eff.get_uniform_location("MVP"), 1, GL_FALSE,
			value_ptr(LP * LV * M));

		// render mesh
		renderer::render(m);
	}

	// set cull face to back
	glCullFace(GL_BACK);
	// set render target to screen
	renderer::set_render_target();
}

bool render() {
	// declaire mvp variables
	mat4 M = skybox.get_transform().get_transform_matrix();
	mat4 VP;

	// set VP to be correct for camera
	switch (use_cam) {
	case 1:
		VP = ABcam.get_projection() * ABcam.get_view();
		break;
	default:
		VP = cam.get_projection() * cam.get_view();
		break;
	}

	// calculate MVP
	mat4 MVP = VP * M;

	if (!wireframe_on)
	{
		// render shadows if shadows are on
		if (shadow_on) {
			renderShadows();
		}
		//FIRSTPASS//
		renderer::set_render_target(frame);
		renderer::clear();
		renderSkyBox(MVP);
		renderer::bind(simple);
		renderSimpleMeshes(VP, simple);

		//SECOND PASS//
		renderEdges();

		//THIRD PASS//
		renderer::set_render_target(frame);
		renderer::clear();
		renderSkyBox(MVP);

		//render normals if normals are on
		if (normals_on)
		{
			renderer::bind(norms);
			//bind colour
			glUniform4fv(norms.get_uniform_location("col"), 1, value_ptr(vec4(0.0, 1.0, 0.0, 1.0)));
			renderSimpleMeshes(VP, norms);
		}
		//render attempted outlines if they're on
		if (atmptLines_on)
		{
			renderer::bind(silh);
			//bind colour
			glUniform4fv(silh.get_uniform_location("col"), 1, value_ptr(vec4(0.0, 0.0, 0.0, 1.0)));
			// set cam pos to be correct for camera
			switch (use_cam) {
			case 1:
				glUniform3fv(silh.get_uniform_location("cam_pos"), 1, value_ptr(ABcam.get_position()));
				break;
			default:
				glUniform3fv(silh.get_uniform_location("cam_pos"), 1, value_ptr(cam.get_position()));
				break;
			}
			//set line width
			glUniform1f(silh.get_uniform_location("line_width"), 0.4f);
			renderSimpleMeshes(VP, silh);
		}
		//render all meshes with lighting
		renderMeshes(VP);
	}
	else
	{
		//FIRSTPASS//
		renderer::set_render_target(mask);
		//set clear colour to white so that mask doesn't colour wireframe
		renderer::setClearColour(1.0, 1.0, 1.0);
		renderer::clear();
		renderer::set_render_target(frame);
		renderer::clear();
		renderer::setClearColour(0.0, 1.0, 1.0);
		renderSkyBox(MVP);

		//render wireframe
		renderer::bind(wires);
		glUniform4fv(wires.get_uniform_location("col"), 1, value_ptr(vec4(1.0, 0.0, 0.0, 1.0)));
		renderSimpleMeshes(VP, wires);
		//render normals if normals are on
		if (normals_on)
		{
			renderer::bind(norms);
			glUniform4fv(norms.get_uniform_location("col"), 1, value_ptr(vec4(0.0, 1.0, 0.0, 1.0)));
			renderSimpleMeshes(VP, norms);
		}
	}

	//SCREEN PASS//
	renderMasking();

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