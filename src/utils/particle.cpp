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

Particle::Particle(const Particle &Particle)
: Particle(Particle.m_position, Particle.m_mass)
{

}

PFixedParticle::PFixedParticle(const glm::vec3 &xyz, const float mass)
: Particle(xyz, mass)
{

}

PFixedParticle::PFixedParticle(const float x, const float y, const float z, const float mass)
: PFixedParticle(glm::vec3(x, y, z), mass)
{

}