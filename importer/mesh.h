#ifndef MESH_H_
#define MESH_H_

#include <vector>

#include "shader.h"
#include "texture.h"
#include "vertex.h"

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int VAO;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

    void Draw(Shader &shader);

private:
    unsigned int VBO, EBO;
    void setupMesh();
};

#endif