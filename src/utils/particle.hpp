#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <functional>
#include <iostream>

class Particle
{
  public:
    // CONSTRUCTORS
    Particle(const glm::vec3 &xyz, const float mass);
    Particle(const float x, const float y, const float z, const float mass);
    Particle(const Particle& particle);

    // GETTERS
    inline const glm::vec3 getPosition() const { return m_position; };
    inline const glm::vec3 getSpeed() const { return m_speed; };
    inline const glm::vec3 getForce() const { return m_force; };
    inline const float getMass() const { return m_mass; };

    // METHODS
    inline void applyForce(const glm::vec3 &force) { m_force += force; };
    void executeleap(const float h) {
      leapFrog(h);
    }
    inline void clearForce() { m_force = glm::vec3(0.); }
    void leapFrog(const float h);
    void eulerExplicit(const float h);

  protected:
    glm::vec3 m_position, m_speed, m_force;
    float m_mass;
};

class PFixedParticle : public Particle
{
  public:
    // CONSTRUCTORS
    PFixedParticle(const glm::vec3 &xyz, const float mass);
    PFixedParticle(const float x, const float y, const float z, const float mass);

    // METHODS
    inline void applyForce(const glm::vec3 &force) {};
    inline void executeleap(const float h) {};

};