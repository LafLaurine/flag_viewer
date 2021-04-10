#include "lights.hpp"

void DirectionalLight::loadDefaults()
{
  m_color = glm::vec3(1, 1, 1);
  glm::vec3 dir = glm::vec3(1.f, 0.f, 0.f);
  m_intensity = glm::vec3(10.f, 10.f, 10.f);
  setDirection(dir);
}

glm::vec3 DirectionalLight::computeDirection(float theta, float phi)
{
  const auto sinPhi = glm::sin(phi);
  const auto cosPhi = glm::cos(phi);
  const auto sinTheta = glm::sin(theta);
  const auto cosTheta = glm::cos(theta);
  return glm::vec3(sinTheta * cosPhi, cosTheta, sinTheta * sinPhi);
}

glm::vec3 DirectionalLight::computeEulerAngles(glm::vec3 dir)
{
  const glm::quat quat = glm::rotation(glm::vec3(), dir);
  return glm::eulerAngles(quat);
}

void DirectionalLight::setDirection(glm::vec3 a_dir)
{
  m_dir = a_dir;
  const glm::vec3 euler = computeEulerAngles(m_dir);
  m_theta = euler.x;
  m_phi = euler.y;
}

void DirectionalLight::setDirection(float theta, float phi)
{
  m_theta = theta;
  m_phi = phi;
  m_dir = computeDirection(m_theta, m_phi);
}