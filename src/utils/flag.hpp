#ifndef Flag_SIM_H
#define Flag_SIM_H

#include "GLFWHandle.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>

#include "particle.hpp"
#include "damper.hpp"
#include "triangle.hpp"
#include <vector>

// Constants
#define Flag_WIDTH 50
#define Flag_HEIGHT 50
#define SAMPLES_PER_FRAME 5

#define CAMERA_ZOOM 8 
#define FLOORHEIGHT -2

extern glm::vec3 wind;

const float SPRING_CONST = 500.f;
const float DAMP_CONST = 5.f;

class Flag
{
public:
	GLuint VAO, VBO, NBO, EBO;
	std::vector<GLuint> indices;

	std::vector<Particle*> particles;
	std::vector<Damper*> dampers;
	std::vector<Triangle*> triangles;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;

	Flag();
	~Flag();

	void rebind();
	void genNormals();
	void genWind();
	void genDamp();
	void translate(glm::vec3 dist);
	void updateParticles();
	void draw();
	void update();
};

#endif