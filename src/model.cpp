#include "model.hpp"

#include "log.hpp"

#include "config.hpp"


Model::Model(std::string name)
{
  std::string err;
  bool ret = tinyobj::LoadObj(
      shapes, 
      materials, 
      err, 
      (DATA_FOLDER+name).c_str()
  ); 

  if (!err.empty()) {
    Log::w("OBJ loading error: %s", err.c_str());
  }
  if (!ret) {
    Log::e("Error while loading model file %s!\n", name.c_str());
  }

  this->name = name;
}

void Model::generate()
{
  Log::i("Generating model %s buffers...\n", this->name.c_str());

  for (const auto &shape : shapes)
  {
    // TODO: refactor
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(
        GL_ARRAY_BUFFER, 
        shape.mesh.positions.size() * sizeof(GL_FLOAT),
        &shape.mesh.positions.at(0),
        GL_STATIC_DRAW);
    vertexBuffers.push_back(vertexBuffer);

    GLuint uvBuffer;
    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    std::vector<float> texcoords;
    texcoords.reserve(shape.mesh.texcoords.size());
    for (size_t i = 0; i < shape.mesh.texcoords.size(); i++)
    {
      if (i % 2 != 0)
        texcoords.push_back(1.0 - shape.mesh.texcoords.at(i));
      else
        texcoords.push_back(shape.mesh.texcoords.at(i));
    }
    if (texcoords.size() > 0)
    {
      glBufferData(GL_ARRAY_BUFFER, 
          texcoords.size() * sizeof(GL_FLOAT),
          &texcoords.at(0),
          GL_STATIC_DRAW);
      uvBuffers.push_back(uvBuffer);
    } else
    {
      uvBuffers.push_back(0);
    }

    GLuint normalBuffer;
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, 
        shape.mesh.normals.size() * sizeof(GL_FLOAT),
        &shape.mesh.normals.at(0),
        GL_STATIC_DRAW);
    normalBuffers.push_back(normalBuffer);
    
    GLuint elementBuffer;
    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, 
        shape.mesh.indices.size() * sizeof(GL_UNSIGNED_INT), //GL_UNSIGNED_INT
        &shape.mesh.indices[0], 
        GL_STATIC_DRAW);
    indexBuffers.push_back(elementBuffer);

    Log::i("Model %s shape loaded. v=%lu uv=%lu n=%lu i=%d\n", 
        name.c_str(), shape.mesh.positions.size(),
        shape.mesh.texcoords.size(), shape.mesh.normals.size(),
        shape.mesh.indices.size());
  }
  
}

void Model::draw()
{
  for (size_t i = 0; i < vertexBuffers.size(); ++i)
  {
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers.at(i));
    glVertexAttribPointer(
        0,
        3, 
        GL_FLOAT,
        GL_FALSE,
        0,
        NULL
    );

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffers.at(i));
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        0,
        NULL
    );

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffers.at(i));
    glVertexAttribPointer(
        2,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        NULL
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers.at(i));
    
    glDrawElements(
        GL_TRIANGLES,
        shapes.at(0).mesh.indices.size(),
        GL_UNSIGNED_INT,
        NULL
    );

    //glDrawArrays(GL_TRIANGLES, 0, (GLsizei)shapes[0].mesh.positions.size());

    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

  }
}

Model::~Model()
{
}
