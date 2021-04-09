#ifndef PARTICLE_H_
#define PARTICLE_H_

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Particle {
public:
	Particle(glm::vec3 pos, float mass, float damping, glm::vec2 texCoord);

	bool isStatic;
	glm::vec3 normal;
	glm::vec3 pos;
	glm::vec3 lastPos;
	glm::vec2 texCoord;
	float damping;
	float mass;
	glm::vec3 force;

	void move(glm::vec3 delta);
	void step(float timeStep);

};

#endif