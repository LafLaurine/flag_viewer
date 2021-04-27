#include "flag.hpp"

static glm::vec3 const g(0.0f, -9.81f, 0.0f);

static glm::vec3 springForce(float const K, glm::vec3 const& u, float const L_rest)
{
	float const L = glm::l2Norm(u);
	glm::vec3 spring_f = K * (L - L_rest) * u / L;
	return spring_f;
}


Flag::Flag(int width, int height, const GLProgram& program) :
	Nu(width), Nv(height), m_forces(Nu*Nv, glm::vec3(0.0f)),
	m_speed(Nu*Nv, glm::vec3(0.0f)), m_mass(1.0f),
	is_wind(false), wind_strength(0.001f), wind_direction(glm::vec3(0.0f, 0.0f, 1.0f))
{
	initMesh();

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*m_vertices.size(), &m_vertices[0], GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(glm::vec3)));

	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*m_indices.size(), &m_indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Flag::~Flag()
{
	if (m_vao != 0)
	{
		glDeleteVertexArrays(1, &m_vao);
	}

	if (m_vbo != 0)
	{
		glDeleteBuffers(1, &m_vbo);
	}

	if (m_ibo != 0)
	{
		glDeleteBuffers(1, &m_ibo);
	}
}

void Flag::initMesh()
{
	float step_u = 1.0f / (Nu-1);
	float step_v = 1.0f / (Nv-1);

	for (int j = 0; j < Nv; j++)
	{
		for (int i = 0; i < Nu; i++)
		{
			glm::vec3 position(-0.5f + step_u * (float)i, 0.0f, -0.5f + step_v * (float)j);
			glm::vec3 normal(0.0f);

			Vertex vertex; 
			vertex.position = position; 
			vertex.normal = normal; 
			m_vertices.push_back(vertex);

			if (j < Nv-1 && i < Nu-1)
			{
				// Triangle 1
				m_indices.push_back(index(i, j));
				m_indices.push_back(index(i + 1, j));
				m_indices.push_back(index(i + 1, j + 1));

				// Triangle 2
				m_indices.push_back(index(i + 1, j + 1));
				m_indices.push_back(index(i, j + 1));
				m_indices.push_back(index(i, j));
			}
		}
	}

	updateNormals();

	assert(m_indices.size() == (Nu-1) * (Nv-1) * 2 * 3 && m_vertices.size() == Nu*Nv);
}

void Flag::updateForces(float K_struct,float K_shear, float K_bend,float K_wind)
{
	int const N_total = Nu * Nv;
	glm::vec3 const g_normalized = g / (float)N_total;

	static float L_rest = 1.0f / (Nu - 1.0f);
	static float L_diag = sqrt(pow(1.0f / (Nu - 1.0f), 2) + pow(1.0f / (Nv - 1.0f), 2));
	static float L_far = 2.0f / (Nu - 1.0f);

	for (int kv = 0; kv < Nv; kv++)
	{
		for (int ku = 0; ku < Nu; ku++)
		{
			glm::vec3 F = glm::vec3(0.0f);
			
			if (ku > 0)
			{
				glm::vec3 u_left = getPosition(ku - 1, kv) - getPosition(ku, kv);
				F += springForce(K_struct, u_left, L_rest);
			}
			if (kv > 0)
			{
				glm::vec3 u_down = getPosition(ku, kv - 1) - getPosition(ku, kv);
				F += springForce(K_struct, u_down, L_rest);
			}
			if (ku < Nu-1)
			{
				glm::vec3 u_right = getPosition(ku + 1, kv) - getPosition(ku, kv);
				F += springForce(K_struct, u_right, L_rest);
			}
			if (kv < Nv - 1)
			{
				glm::vec3 u_up = getPosition(ku, kv + 1) - getPosition(ku, kv);
				F += springForce(K_struct, u_up, L_rest);
			}
			if (ku > 0 && kv > 0)
			{
				glm::vec3 u_down_left = getPosition(ku-1, kv - 1) - getPosition(ku, kv);
				F += springForce(K_shear, u_down_left, L_diag);
			}
			if (ku < Nu-1 && kv < Nv-1)
			{
				glm::vec3 u_up_right = getPosition(ku + 1, kv + 1) - getPosition(ku, kv);
				F += springForce(K_shear, u_up_right, L_diag);
			}
			if (ku > 0 && kv < Nv - 1)
			{
				glm::vec3 u_up_left = getPosition(ku - 1, kv + 1) - getPosition(ku, kv);
				F += springForce(K_shear, u_up_left, L_diag);
			}
			if (ku < Nu-1 && kv > 0)
			{
				glm::vec3 u_down_right = getPosition(ku + 1, kv - 1) - getPosition(ku, kv);
				F += springForce(K_shear, u_down_right, L_diag);
			}
			if (ku > 1)
			{
				glm::vec3 u_far_left = getPosition(ku - 2, kv) - getPosition(ku, kv);
				F += springForce(K_bend, u_far_left, L_far);
			}
			if (kv > 1)
			{
				glm::vec3 u_far_down = getPosition(ku, kv - 2) - getPosition(ku, kv);
				F += springForce(K_bend, u_far_down, L_far);
			}
			if (ku < Nu-2)
			{
				glm::vec3 u_far_right = getPosition(ku + 2, kv) - getPosition(ku, kv);
				F += springForce(K_bend, u_far_right, L_far);
			}
			if (kv < Nv - 2)
			{
				glm::vec3 u_far_up = getPosition(ku, kv + 2) - getPosition(ku, kv);
				F += springForce(K_bend, u_far_up, L_far);
			}

			if (is_wind)
			{
				glm::vec3 normalized_wind_dir;
				if (wind_direction == glm::vec3(0.0f, 0.0f, 0.0f))
					normalized_wind_dir = glm::vec3(0.0f, 0.0f, 0.0f);
				else
					normalized_wind_dir = normalize(wind_direction);
				glm::vec3 normal = getNormal(ku, kv);
				glm::vec3 f_wind = wind_strength * K_wind * glm::dot(normal, normalized_wind_dir) * normal;
				F += f_wind;
			}

			m_forces[index(ku, kv)] = F + g_normalized;
		}
	}
	
	//Fixed point must not be impacted by forces
	m_forces[index(0, Nv-1)] = glm::vec3(0.0f);
	m_forces[index(Nu-1, Nv-1)] = glm::vec3(0.0f);

	securityCheck();
}

void Flag::updatePosition(float const dt)
{
	assert(m_forces.size() == m_speed.size());
	assert(m_forces.size() == m_vertices.size());

	float const mu = 0.1f;

	for (int ku = 0; ku < Nu; ku++)
	{
		for (int kv = 0; kv < Nv; kv++)
		{
			glm::vec3 force = m_forces[index(ku, kv)];
			glm::vec3 speed = m_speed[index(ku, kv)];

			speed = (1-mu*dt)*speed + dt * force;
			m_speed[index(ku, kv)] = speed;

			m_vertices[index(ku, kv)].position += speed * dt;

		}
	}

	securityCheck();
	
	updateNormals();

	updateBuffer();
}

void Flag::updateNormals()
{
	glm::vec3 up;
	glm::vec3 left;

	for (int kv = 0; kv < Nv; kv++)
	{
		for (int ku = 0; ku < Nu; ku++)
		{
			glm::vec3 normal = glm::vec3(0.0f);

			if (ku > 0 && kv > 0)
			{
				up = getPosition(ku - 1, kv) - getPosition(ku, kv);
				left = getPosition(ku, kv - 1) - getPosition(ku, kv);
				normal += glm::cross(up, left);
			}
			if (ku < Nu - 1 && kv < Nv - 1)
			{
				up = getPosition(ku + 1, kv) - getPosition(ku, kv);
				left = getPosition(ku, kv + 1) - getPosition(ku, kv);
				normal += glm::cross(up, left);
			}
			if (ku > 0 && kv < Nv - 1)
			{
				up = getPosition(ku, kv + 1) - getPosition(ku, kv);
				left = getPosition(ku - 1, kv) - getPosition(ku, kv);
				normal += glm::cross(up, left);
			}
			if (ku < Nu - 1 && kv > 0)
			{
				up = getPosition(ku, kv - 1) - getPosition(ku, kv);
				left = getPosition(ku + 1, kv) - getPosition(ku, kv);
				normal += glm::cross(up, left);
			}

			m_vertices[index(ku, kv)].normal = glm::normalize(normal);
		}
	}

	assert(m_vertices.size() == Nu * Nv);
}

void Flag::updateBuffer()
{
	assert(m_vbo != -1 && m_vbo != 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex)*m_vertices.size(), &m_vertices[0]);
}

void Flag::render()
{
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, (void*)0);
}

unsigned int Flag::index(int i, int j)
{
	assert(i >= 0 && i < Nu && j >= 0 && j < Nv);
	return (unsigned int)(j * Nu + i);
}

glm::vec3 Flag::getPosition(int i, int j)
{
	assert(i >= 0 && i < Nu && j >= 0 && j < Nv);

	int a = j * Nu + i;

	return m_vertices[a].position;
}

glm::vec3 Flag::getForces(int i, int j)
{
	assert(i >= 0 && i < Nu && j >= 0 && j < Nv);

	int a = j * Nu + i;

	return m_forces[a];
}

glm::vec3 Flag::getNormal(int i, int j)
{
	assert(i >= 0 && i < Nu && j >= 0 && j < Nv);

	int a = j * Nu + i;

	return m_vertices[a].normal;
}

void Flag::securityCheck()
{
	// security check for divergence

	static float const LIMIT = 30.0f;

	for (int ku = 0; ku < Nu; ku++)
	{
		for (int kv = 0; kv < Nv; kv++)
		{
			glm::vec3 const& p = getPosition(ku, kv);

			if (glm::l2Norm(p) > LIMIT)
				exit(2);
		}
	}
}
