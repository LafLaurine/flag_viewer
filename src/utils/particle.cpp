#include "particle.hpp"
#include "flag.hpp"

void Particle::update(){
	// Do nothing if fixed
	if(fixed) return;

	//compute force done by gravity
	force += mass * GRAVITY;

	//euler integration to get velocity and pos
	velocity += TIMER * (force / mass);
	position += TIMER * velocity;

	collisionCheck();

	//zero out force
	force = glm::vec3(0);
}

void Particle::collisionCheck(){
	if(position.y < FLOORHEIGHT){
		position.y = 2 * FLOORHEIGHT - position.y;
		velocity.y = -ELASTICITY * velocity.y;
		velocity.x = (1 - FRICTION) * velocity.x;
		velocity.z = (1 - FRICTION) * velocity.z;
	}
}

void Particle::applyForce(glm::vec3 f){
	force += f;
}

