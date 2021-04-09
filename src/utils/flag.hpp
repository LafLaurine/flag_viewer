#ifndef Flag_H_
#define Flag_H_

#include "GLFWHandle.hpp"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/fast_square_root.hpp"
#include <vector>

#include "particle.hpp"
#include "constraint.hpp"

class Flag {
public:
	Flag(float width, float height);
	void loadTexture(char *filename);
	void displayConstraints();
	void step(float timeStep);
	std::vector<Particle> particles;

	std::vector<Constraint> constraints;
	std::vector<GLuint> indexes;
	GLuint vao;
	GLuint vbo;
	GLuint ibo;

	void initFlagVertex();
private:
	int width;
	int height;
};

#endif
