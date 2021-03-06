#pragma once

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/norm.hpp>

#include "GLFWHandle.hpp"
#include "shaders.hpp"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
};

class Flag {

private:
	//vertex objects
	unsigned int m_vao, m_vbo, m_ibo;
	//mass of the flag
	float m_mass;

	//grid for the flag
	int flagHeight, flagWidth;
	std::vector<unsigned int> m_indexes;
	std::vector<glm::vec3> m_speed;

public:
	Flag(int width, int height, const GLProgram& program);
	~Flag();
		
	std::vector<Vertex> m_vertices;
	bool is_wind;
	float wind_strength;
	glm::vec3 wind_direction;
	std::vector<glm::vec3> m_forces;

	void initMesh();
	void updateForces(float K_struct,float K_shear, float K_bend,float K_wind);
	void updatePosition(float const dt);
	void updateNormals();
	void updateBuffer();
	void render();
	unsigned int index(int i, int j);

	glm::vec3 getPosition(int i, int j);
	glm::vec3 getNormal(int i, int j);
	glm::vec3 getForces(int i, int j);

	void securityCheck();
};