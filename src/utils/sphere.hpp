#include <vector>
#include "GLFWHandle.hpp"
#include <glm/glm.hpp>

class Sphere {
public:
    void build(GLfloat radius, GLsizei discLat, GLsizei discLong);
    void updatePosition(int index,glm::vec3 pos);

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
    };

    std::vector<glm::vec3> m_positions;
    GLfloat m_radius;

    Sphere(GLfloat radius, GLsizei discLat, GLsizei discLong):
        m_vertexCount(0) {
        build(radius, discLat, discLong);
    }

    // get vertices data
    const Vertex* getDataPointer() const {
        return &m_vertices[0];
    }

    // get number of vertices
    GLsizei getVertexCount() const {
        return m_vertexCount;
    }

private:
    std::vector<Vertex> m_vertices;
    GLsizei m_vertexCount;
};

struct SphereHandler {
    std::vector<glm::vec3> positions;
    std::vector<float> radius;
};