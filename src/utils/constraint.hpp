#ifndef CONSTRAINT_H_
#define CONSTRAINT_H_

#include "particle.hpp"

class Constraint {
public:
	Constraint(Particle *p1, Particle *p2);

	void satisfy();

private:
	Particle *p1, *p2;
	float distance;
};

#endif 