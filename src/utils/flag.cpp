#include "flag.hpp"


struct FlagVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

Flag::Flag(float width, float height) {
	this->width = width;
	this->height = height;
}

void Flag::initFlagVertex() {

  std::vector<FlagVertex> data;

  for(size_t i = 0; i < width; ++i) {
    for(size_t j = 0; j < height; ++j) {
      FlagVertex vertex;
            
      vertex.texCoords.x = float(i) / float(width);
      vertex.texCoords.y = float(j) / float(height);

      vertex.normal = glm::vec3(0, 0, 1); // perpendicular with flag
      
      vertex.position = glm::vec3(i - float(width) / 2., j - float(height) / 2., 0.) * 0.5f;

      data.push_back(vertex);
    }
  }

  for(size_t i = 0; i < width - 1; ++i) {
    size_t offset = i * height;
    for(size_t j = 0; j < height - 1; ++j) {
        indexes.push_back(offset + j);
        indexes.push_back(offset + j + 1);
        indexes.push_back(offset + height + j + 1);
        indexes.push_back(offset + j);
        indexes.push_back(offset + height + j);
        indexes.push_back(offset + height + j + 1);
    }
  }


  // Generate VAO
  glGenVertexArrays(1, &vao);

  // Bind VAO
  glBindVertexArray(vao);

  const GLuint VERTEX_ATTR_POSITION = 0;
  const GLuint VERTEX_ATTR_NORMAL = 1;
  const GLuint VERTEX_ATTR_TEXTURE = 2;
  glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
  glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
  glEnableVertexAttribArray(VERTEX_ATTR_TEXTURE);

   // Generate VBO
  glGenBuffers(1, &vbo);

  // Bind VBO to VAO
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // Insert Data
  glBufferData(
      GL_ARRAY_BUFFER,
      data.size() * sizeof(FlagVertex),
      &data[0],
      GL_DYNAMIC_DRAW
  );

  // Generate IBO
  glGenBuffers(1, &ibo);

  // Bind IBO to VAO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), &indexes[0], GL_STATIC_DRAW);

  //Specify vertex attributes
  glVertexAttribPointer(
      VERTEX_ATTR_POSITION,
      3,
      GL_FLOAT,
      GL_FALSE,
      sizeof(FlagVertex),
      (const GLvoid*)(offsetof(FlagVertex, position))
  );

  glVertexAttribPointer(
      VERTEX_ATTR_NORMAL,
      3,
      GL_FLOAT,
      GL_FALSE,
      sizeof(FlagVertex),
      (const GLvoid*)(offsetof(FlagVertex, normal))
  );

  glVertexAttribPointer(
      VERTEX_ATTR_TEXTURE,
      2,
      GL_FLOAT,
      GL_FALSE,
      sizeof(FlagVertex),
      (const GLvoid*)(offsetof(FlagVertex, texCoords))
  );


  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}