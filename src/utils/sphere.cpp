#include <cmath>
#include <vector>
#include <iostream>
#include <glm/gtc/constants.hpp>
#include "sphere.hpp"

void Sphere::build(GLfloat m_radius, GLsizei lat, GLsizei longi) {
    // Sphere equation (r, phi, theta)
    // r >= 0, -PI / 2 <= theta <= PI / 2, 0 <= phi <= 2PI

    GLfloat rcpLat = 1.f / lat;
    GLfloat rcpLong = 1.f / longi;
    GLfloat dPhi = 2 * glm::pi<float>() * rcpLat, dTheta = glm::pi<float>() * rcpLong;
    GLfloat r = m_radius;
    
    std::vector<Vertex> data;
    
    for(GLsizei j = 0; j <= longi; ++j) {
        GLfloat cosTheta = cos(-glm::pi<float>() / 2 + j * dTheta);
        GLfloat sinTheta = sin(-glm::pi<float>() / 2 + j * dTheta);
        
        for(GLsizei i = 0; i <= lat; ++i) {
            Vertex vertex;
            vertex.normal.x = sin(i * dPhi) * cosTheta;
            vertex.normal.y = sinTheta;
            vertex.normal.z = cos(i * dPhi) * cosTheta;
            
            vertex.position = r * vertex.normal;
            
            data.push_back(vertex);
        }
    }

    m_vertexCount = lat * longi * 6;
    
    GLuint idx = 0;

    for(GLsizei j = 0; j < longi; ++j) {
        GLsizei offset = j * (lat + 1);
        for(GLsizei i = 0; i < lat; ++i) {
            m_vertices.push_back(data[offset + i]);
            m_vertices.push_back(data[offset + (i + 1)]);
            m_vertices.push_back(data[offset + lat + 1 + (i + 1)]);
            m_vertices.push_back(data[offset + i]);
            m_vertices.push_back(data[offset + lat + 1 + (i + 1)]);
            m_vertices.push_back(data[offset + i + lat + 1]);
        }
    }

    for(auto& vertice: m_vertices) {
        m_positions.push_back(vertice.position);
    }
}

void Sphere::updatePosition(int index, glm::vec3 pos) {
    m_positions[index] = pos;
}