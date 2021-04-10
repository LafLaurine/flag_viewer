#include "link.hpp"

Link::Link(const Particle &p1, const Particle &p2)
: m_p1(p1),
  m_p2(p2),
  m_k(0.f),
  m_z(0.f),
  m_l(p2.getPosition() - p1.getPosition())
{

}

Link::Link(const Link &Link)
: m_p1(Link.m_p1),
  m_p2(Link.m_p2),
  m_k(Link.m_k),
  m_z(Link.m_z),
  m_l(Link.m_l)
{

}

float Link::s_k(0.);
float Link::s_z(0.);

void Link::hook(const Link &link) {
  glm::vec3 d = link.m_p2.getPosition() - link.m_p1.getPosition();
  glm::vec3 f = (link.m_k + s_k) * (d - link.m_l); // raideur * allongement
  // distrib
  link.m_p1.applyForce(f);
  link.m_p2.applyForce(-f);
}

void Link::brake(const Link &link) {
  glm::vec3 s = link.m_p2.getSpeed() - link.m_p1.getSpeed();
  glm::vec3 f = (link.m_z + s_z) * s;
  // distrib
  link.m_p1.applyForce(f);
  link.m_p2.applyForce(-f);
}

void Link::hookBrake(const Link &link) {
  hook(link);
  brake(link);
}