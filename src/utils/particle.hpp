#pragma once

#include "GLFWHandle.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>

class Particle{
public:

	const glm::vec3 GRAVITY = glm::vec3(0.f, -9.8f, 0.f);
	const float TIMER = 1.f/3000.f;
	const float ELASTICITY = 0.05f;
	const float FRICTION = 0.80f;

	float mass;
	glm::vec3 velocity = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 force =  glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 position;

	GLuint index;
	bool fixed;

	void update();
	void applyForce(glm::vec3);
	void collisionCheck();
};
