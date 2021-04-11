#include "damper.hpp"

void Damper::computeForce(){
	glm::vec3 direction = p2->position - p1->position;

	glm::vec3 dir = glm::normalize(direction);

	//force = -k * <dir, v1> - d *<dir, v2>
	float v1 = glm::dot(dir, p1->velocity);
	float v2 = glm::dot(dir, p2->velocity);
	float spring_force = -k * (length - glm::length(direction)) - d * (v1 - v2);

	//apply force with direction
	glm::vec3 force = spring_force * dir;
	p1->applyForce(force);
	p2->applyForce(-force);
}

