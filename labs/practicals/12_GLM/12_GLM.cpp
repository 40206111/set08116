#include <glm\glm.hpp>
#include <glm\gtc\constants.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <glm\gtx\projection.hpp>
#include <iostream>

using namespace std;
using namespace glm;

int main()
{
	vec2 u(2.0f, 0.0f);
	vec2 v(0.0f, 6.0f);

	vec3 w(0.0f, 3.0f, 0.0f);
	vec3 x(1.0f, 0.0f, 0.0f);

	vec4 y(0.0f, 0.0f, 1.0f, 0.0f);
	vec4 z(4.0f, 0.0f, 0.0f, 0.0f);

	vec2 a(w);
	vec2 b(y);

	vec3 c(u, 1.0f);
	vec3 d(z);

	vec4 e(v, 0.0f, 0.0f);
	vec4 f(x, 0.0f);

	vec2 g = u + v;
	vec2 h = a - b;

	vec3 i = w + x;
	vec3 j = c - d;

	vec4 k = y + z;
	vec4 l = e - f;

	vec2 m = u * 1.0f;
	vec2 n = a / 1.0f;

	vec3 o = w * 1.0f;
	vec3 p = c / 1.0f;

	vec4 q = y * 1.0f;
	vec4 r = e / 1.0f;

	float s = length(u);

	float t = length(w);

	float U = length(y);

	vec2 V = normalize(v);

	vec3 W = normalize(x);

	vec4 X = normalize(z);

	float Y = dot(a, b);

	float Z = dot(c, d);

	float A = dot(e, f);

	vec2 B = proj(g, h);

	vec3 C = proj(i, j);

	vec4 D = proj(k, l);

	vec3 E = cross(o, p);


	mat3 F(1.0f);
	mat4 G(1.0f);
	mat4 H(2.0f);

	G[0][1] = 99.0f;
	G[0][1] = 0.0f;

	mat4 I = G + H;

	mat4 J = I * 72.0f;

	mat4 K = I * J;

	vec4 L = J * e;

	mat4 M = translate(G, x);

	vec3 N = M * vec4(d, 1.0f);


}