#include "triangle.hpp"
#include "flag.hpp"

Triangle::Triangle(Particle * p1, Particle * p2, Particle * p3){
	Triangle::p1 = p1;
	Triangle::p2 = p2;
	Triangle::p3 = p3;
}

void Triangle::applyWind(){
	glm::vec3 velocity = (p1->velocity + p2->velocity + p3->velocity)/3.f - wind;
	glm::vec3 normal = glm::cross((p2->position - p1->position), (p3->position - p1->position));

	float area = 0.5 * glm::length(normal);
	normal = glm::normalize(normal) * glm::dot(glm::normalize(velocity), normal);

	//split the force equally to the 3 particle
	glm::vec3 force = (-0.5f * AIR_DENSITY*DRAG_COEFF * area * powf(glm::length(velocity), 2) * normal) / 3.f;
	p1->applyForce(force);
	p2->applyForce(force);
	p3->applyForce(force);
}