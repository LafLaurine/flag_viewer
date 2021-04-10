#include "particle.hpp"

Particle::Particle(const glm::vec3 &xyz, const float mass)
: m_position(xyz),
  m_speed(0, 0, 0),
  m_force(0, 0, 0),
  m_mass(mass)
{

}

Particle::Particle(const float x, const float y, const float z, const float mass)
: Particle(glm::vec3(x, y, z), mass)
{

}



Particle::Particle(const Particle &particle)
: Particle(particle.m_position, particle.m_mass)
{

}


void Particle::leapFrog(const float h) {
    m_speed += h * m_force / m_mass;
    m_position += h * m_speed;
}

void Particle::eulerExplicit(const float h) {
    m_position += h * m_speed;
    m_speed += h * m_force / m_mass;
}

PFixedParticle::PFixedParticle(const glm::vec3 &xyz, const float mass)
: Particle(xyz, mass)
{

}

PFixedParticle::PFixedParticle(const float x, const float y, const float z, const float mass)
: PFixedParticle(glm::vec3(x, y, z), mass)
{

}