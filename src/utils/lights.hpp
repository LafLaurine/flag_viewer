#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>

class DirectionalLight
{
public:
  DirectionalLight() { loadDefaults(); };

  inline const glm::vec3 &getDirection() const { return m_dir; }
  inline glm::vec3 getRadiance() const { return m_color * m_intensity; }
  inline const glm::vec3 &getColor() const { return m_color; }
  inline glm::vec3 getIntensity() const { return m_intensity; }
  inline glm::vec2 getEulerAngles() const { return glm::vec2(m_theta, m_phi); }
  inline void setColor(glm::vec3 color) { m_color = color; }
  inline void setIntensity(glm::vec3 intensity) { m_intensity = intensity; }

  void setDirection(glm::vec3 a_dir);

  void setDirection(float theta, float phi);

private:
  glm::vec3 m_color;
  glm::vec3 m_intensity;
  glm::vec3 m_dir;
  float m_theta;
  float m_phi;

  void loadDefaults();
  glm::vec3 computeDirection(float theta, float phi);
  glm::vec3 computeEulerAngles(glm::vec3 dir);
};