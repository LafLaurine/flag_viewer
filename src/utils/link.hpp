#include <glm/glm.hpp>
#include "particle.hpp"

class Link
{
  public:

    // CONSTRUCTORS
    Link(const Particle &p1, const Particle &p2);
    Link(Particle *const p1, Particle *const p2);
    Link(const Link &Link);

    // STATIC ATTRIBUTES
    static float s_k;
    static float s_z;

    // STATIC METHODS
    void hook(const Link &link);
    void brake(const Link &link);
    void hookBrake(const Link &link);
    void execute() { hookBrake(*this); };

  protected:
    const Particle m_p1, m_p2;
    float m_k; // k, raideur
    glm::vec3 m_l; // l, longueur à vide 
    float m_z; // z, viscosité;
};

