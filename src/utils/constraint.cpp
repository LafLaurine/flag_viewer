#include "constraint.hpp"

Constraint::Constraint(Particle *p1, Particle *p2) {
	this->p1 = p1;
	this->p2 = p2;
	distance = glm::length(p1->pos-p2->pos);
}

void Constraint::satisfy() {
	glm::vec3 v = p1->pos - p2->pos;
	v *= ((1.0f - distance/glm::length(v)) * 0.8f);
	p1->move(-v);
	p2->move(v);
}