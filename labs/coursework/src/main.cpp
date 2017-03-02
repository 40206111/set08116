#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

effect eff;
free_camera cam;
arc_ball_camera ABcam;
map<string, mesh> meshes;
double cursor_x = 0;
double cursor_y = 0;
map<string, texture> tex;
texture shade;
map<string, texture> normal_map;
directional_light light;
point_light point;
spot_light spot;
material mat;
shadow_map shadow;
int use_cam = 0;

bool initialise() {
  // hide cursor
  glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  // capture initial mouse position
  glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);

  return true;
}

bool load_content() {

  // create shadow map
  shadow =
      shadow_map(renderer::get_screen_width(), renderer::get_screen_height());

  // create floating islands
  meshes["floating island"] = mesh(geometry("models/floating island.obj"));
  meshes["island2"] = mesh(geometry("models/island2.obj"));
  meshes["island3"] = mesh(geometry("models/island3.obj"));

  // create castle
  meshes["castle"] = mesh(geometry("models/castle.obj"));

  // translate island2
  meshes["island2"].get_transform().translate(vec3(50.0f, 0.0f, -100.0f));
  // translate island3
  meshes["island3"].get_transform().translate(vec3(-50.0f, 0.0f, -100.0f));
  // transform castle
  meshes["castle"].get_transform().scale = vec3(0.3);
  meshes["castle"].get_transform().translate(vec3(-1.0f, 5.8f, 0.0f));
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

  // set light
  light.set_ambient_intensity(vec4(0.1f));
  light.set_light_colour(vec4(1.0f));
  light.set_direction(normalize(vec3(-1.0, 0.5, 0.0)));

  // set spot light
  spot.set_light_colour(vec4(0.0f, 0.0f, 0.0f, 1.0f));
  spot.set_position(vec3(0.0f, 25.0f, 0.0f));
  spot.set_direction(normalize(-spot.get_position()));
  spot.set_range(20.0f);
  spot.set_power(10.0f);
  spot.set_constant_attenuation(0.1f);

  // set point light
  point.set_light_colour(vec4(0.0f, 0.0f, 0.0f, 1.0f));
  point.set_position(vec3(-6.0f, 3.0f, 0.0f));
  point.set_range(5.0f);

  // set default material
  mat.set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
  mat.set_diffuse(vec4(1.0f));
  mat.set_shininess(10.0f);
  mat.set_specular(vec4(0.0f, 0.0f, 0.0f, 1.0f));

  // set shade data
  vector<vec4> shade_data{vec4(0.0f, 0.0f, 0.0f, 1.0f),
                          vec4(0.25f, 0.25f, 0.25f, 1.0f),
                          vec4(0.5f, 0.5f, 0.5f, 1.0f),
                          vec4(0.75f, 0.75f, 0.75f, 1.0f), vec4(1.0f)};
  // set shade texture
  shade = texture(shade_data, 5, 1, false, false);

  // Set camera properties
  cam.set_position(vec3(3.0f, 5.0f, 30.0f));
  cam.set_target(vec3(0.0f, 0.0f, 0.0f));
  cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f,
                     1000.0f);

  ABcam.set_target(meshes["floating island"].get_transform().position);
  ABcam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f,
                       1000.0f);
  ABcam.move(20.0f);

  return true;
}

void FreeCam(float delta_time) {

  // reset camera position
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_C)) {
    cam.set_position(vec3(3.0f, 5.0f, 30.0f));
    cam.set_pitch(0.0f);
    cam.set_yaw(0.0f);
  }
  // reset camera orientation
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_R)) {
    cam.set_pitch(0.0f);
    cam.set_yaw(0.0f);
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

  // rotate cam
  cam.rotate(delta_x, -delta_y);

  // set speed
  float speed = 30.0f * delta_time;
  // set speed for up and down movement
  float hspeed = 20.0f * delta_time;

  // speed up
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT_CONTROL)) {
    speed = 100.0f * delta_time;
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

  // update cursot position
  cursor_x = current_x;
  cursor_y = current_y;
}

void Arc_ball_cam(float delta_time) {
  static std::array<mesh, 3> targets{meshes["floating island"],
                                     meshes["island2"], meshes["island3"]};
  static int i = 0;
  static bool up = true;
  static bool down = false;
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_N) == GLFW_PRESS && up) {
    up = false;
    down = true;
    i++;
    if (i >= 3) {
      i = 0;
    }
    ABcam.set_target(targets[i].get_transform().position);
  }
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_N) == GLFW_RELEASE && down) {
    up = true;
  }
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

  // change camera distance - up/down
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_W)) {
    ABcam.move(-10.0f * delta_time);
  }
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_S)) {
    ABcam.move(10.0f * delta_time);
  }

  // update camera
  ABcam.update(delta_time);

  // Update cursor pos
  cursor_x = current_x;
  cursor_y = current_y;
}

void LightControl() {
  static bool point_on = false;
  static bool spot_on = false;
  static bool dir_on = true;
  static bool pdown = false;
  static bool odown = false;
  static bool ldown = false;

  // toggle point light
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_P) == GLFW_RELEASE &&
      !pdown) {
    pdown = true;
    if (point_on) {
      point.set_light_colour(vec4(1.0f));
    }
    if (!point_on) {
      point.set_light_colour(vec4(0.0f, 0.0f, 0.0f, 1.0f));
    }
    point_on = !point_on;
  }
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_P) == GLFW_PRESS && pdown) {
    pdown = false;
  }

  // toggle spot light
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_O) == GLFW_RELEASE &&
      !odown) {
    odown = true;
    if (spot_on) {
      spot.set_light_colour(vec4(1.0f, 0.0f, 0.0f, 1.0f));
    }
    if (!spot_on) {
      spot.set_light_colour(vec4(0.0f, 0.0f, 0.0f, 1.0f));
    }
    spot_on = !spot_on;
  }
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_O) == GLFW_PRESS && odown) {
    odown = false;
  }

  // toggle directional light
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_L) == GLFW_RELEASE &&
      !ldown) {
    ldown = true;
    if (dir_on) {
      light.set_light_colour(vec4(1.0f));
    }
    if (!dir_on) {
      light.set_light_colour(vec4(0.0f, 0.0f, 0.0f, 1.0f));
    }
    dir_on = !dir_on;
  }
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_L) == GLFW_PRESS && ldown) {
    ldown = false;
  }
}

bool update(float delta_time) {
  // calculate total time
  static float total_time = 0;
  total_time += delta_time;

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

  // control light
  LightControl();

  if (glfwGetKey(renderer::get_window(), GLFW_KEY_0) && use_cam != 0) {
    use_cam = 0;
  }
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_1) && use_cam != 1) {
    use_cam = 1;
  }

  switch (use_cam) {
  case 1:
    // control Arc ball camera
    Arc_ball_cam(delta_time);
    break;
  default:
    // control camera using free cam
    FreeCam(delta_time);
  }

  return true;
}

bool render() {
  // Bind effect
  renderer::bind(eff);

  // loop through meshes
  for (auto e : meshes) {
    mesh m = e.second;
    // create MVP matrix
    mat4 M = m.get_transform().get_transform_matrix();
    auto V = cam.get_view();
    auto P = cam.get_projection();

    switch (use_cam) {
    case 1:
      V = ABcam.get_view();
      P = ABcam.get_projection();
      break;
    default:
      break;
    }

    auto MVP = P * V * M;

    // set MVP matrix uniform
    glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE,
                       value_ptr(MVP));
    // set model matrix
    glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE,
                       value_ptr(M));

    glUniform1i(eff.get_uniform_location("shade_tex"), 1);
    // set normal matrix
    glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE,
                       value_ptr(m.get_transform().get_normal_matrix()));

    renderer::bind(normal_map["blank"], 2);

    // put castle in transform hierarchy of floating island
    if (e.first == "castle") {
      auto VP = P * V;
      M = meshes["floating island"].get_transform().get_transform_matrix() * M;
      glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE,
                         value_ptr(VP * M));
      // set castle model matrix
      glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE,
                         value_ptr(M));
      // set normal matrix
      glUniformMatrix3fv(
          eff.get_uniform_location("N"), 1, GL_FALSE,
          value_ptr(
              meshes["floating island"].get_transform().get_normal_matrix() *
              m.get_transform().get_normal_matrix()));
      renderer::bind(normal_map["bricks"], 2);
    }
    /*if (e.first == "floating island")
    {
            vec4 light_pos = MVP * vec4(-6.0f, 3.0f, 0.0f, 1.0f);
            point.set_position(vec3(light_pos));
    }*/

    // bind textures
    renderer::bind(tex[e.first], 0);
    renderer::bind(shade, 1);

    glUniform1i(eff.get_uniform_location("normal_map"), 2);
    // set texture uniform
    glUniform1i(eff.get_uniform_location("tex"), 0);
    // set shade uniform
    glUniform1i(eff.get_uniform_location("shade_tex"), 1);
    // bind lights
    renderer::bind(light, "light");
    renderer::bind(spot, "spot");
    renderer::bind(point, "point");
    // bind material
    renderer::bind(mat, "mat");
    // set eye position to camera position
    glUniform3fv(eff.get_uniform_location("eye_pos"), 1,
                 value_ptr(cam.get_position()));

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