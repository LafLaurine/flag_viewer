#include "physics.hpp"

void initFlagPhysics(int height, int width, float mass, std::vector<FlagVertex> data, std::vector<Particle> vparticles, std::vector<Link> vlinks) {
// PHYSICS

  // Immovible extremity
  for(size_t i = 0; i < height; ++i) {
    PFixedParticle pfpoint(data[i].position, 0.);
    vparticles.push_back(std::make_shared<PFixedParticle>(pfpoint));
  }

  // Inside
  for(size_t i = height; i < data.size() - height; ++i) {
    Particle particle(data[i].position, mass);
    vparticles.push_back(std::make_shared<Particle>(particle));
  }

  // Extremity
  for(size_t i = data.size() - height; i < data.size(); ++i) {
    Particle particle(data[i].position, mass * 0.9);
    vparticles.push_back(std::make_shared<Particle>(particle));
  }

  /// Structural Mesh + Diagonal Mesh
  // For fixed Point
  for(size_t j = 0; j < height - 1; ++j) {
    // Horizontal
    vlinks.push_back(Link(vparticles[j], vparticles[height + j]));
    // Diagonal left-top corner to right-bottom corner
    vlinks.push_back(Link(vparticles[j], vparticles[height + j + 1]));
  }

  // For internal
  for (size_t i = 1; i < width - 1; ++i) {
    for (size_t j = 0; j < height - 1; ++j) {
      // Horizontal
      vlinks.push_back(Link(vparticles[i * height + j], vparticles[(i + 1) * height + j]));
      // Verical
      vlinks.push_back(Link(vparticles[i * height + j], vparticles[i * height + j + 1]));
      // Diagonal left-bottom corner to right-top corner
      vlinks.push_back(Link(vparticles[(i - 1) * height + j + 1], vparticles[i * height + j]));
      // Diagonal left-top corner to right-bottom corner
      vlinks.push_back(Link(vparticles[i * height + j], vparticles[(i + 1) * height + j + 1]));
    }
  }

  // For extrema
  for (size_t i = 0; i < width - 1; ++i) {
    // Horizontal
    vlinks.push_back(Link(vparticles[(i + 1) * height - 1], vparticles[(i + 2) * height - 1]));
  }
  
  for (size_t j = 0; j < height - 1; ++j) {
    // Vertical
    vlinks.push_back(Link(vparticles[(width - 1) * height + j], vparticles[(width - 1) * height + j + 1]));
    // Diagonal left-bottom corner to right-top corner
    vlinks.push_back(Link(vparticles[(width - 2) * height + j + 1], vparticles[(width - 1) * height + j]));
  }

  /// Bridge Mesh
  for(size_t i = 0; i < width - 2; ++i) {
    for(size_t j = 0; j < height - 2; ++j) {

      if(i > 0) { // no need for fixed points
        // Vertical Bridge
        vlinks.push_back(Link(vparticles[i * height + j], vparticles[i * height + j + 2]));
      }

      // Horizontal Bridge
      vlinks.push_back(Link(vparticles[i * height + j], vparticles[(i + 2) * height + j]));
      
    }
  }
}