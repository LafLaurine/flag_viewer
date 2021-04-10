#ifndef Flag_H_
#define Flag_H_

#include "GLFWHandle.hpp"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/fast_square_root.hpp"
#include <vector>

struct FlagVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Flag {
public:
	Flag(int width, int height, float mass);
	~Flag();
	std::vector<GLuint> indexes;
	std::vector<FlagVertex> data;
	GLuint vao;
	GLuint vbo;
	GLuint ibo;
	int width;
	int height;
	float mass;
	void initFlagVertex();

private:
	bool isTextured;
	GLuint textureID;
};

#endif
