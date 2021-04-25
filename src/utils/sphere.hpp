#include <vector>
#include "GLFWHandle.hpp"
#include <glm/glm.hpp>

class Sphere {
public:
    void build(GLfloat radius, GLsizei discLat, GLsizei discLong);

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
    };

    std::vector<glm::vec3> m_positions;
    GLfloat m_radius;

    Sphere(GLfloat radius, GLsizei discLat, GLsizei discLong):
        m_nVertexCount(0) {
        build(radius, discLat, discLong);
    }

    // get vertices data
    const Vertex* getDataPointer() const {
        return &m_vertices[0];
    }

    // get number of vertices
    GLsizei getVertexCount() const {
        return m_nVertexCount;
    }

private:
    std::vector<Vertex> m_vertices;
    GLsizei m_nVertexCount;
};