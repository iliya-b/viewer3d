
#ifndef VERTEX_H_
#define VERTEX_H_
#define MAX_BONE_INFLUENCE 4

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Vertex
{

    glm::vec3 Position;

    glm::vec3 Normal;

    glm::vec2 TextureCoords;
     
    
    glm::vec3 Tangent;

    glm::vec3 Bitangent;

    int bone_id[MAX_BONE_INFLUENCE];

    float weights[MAX_BONE_INFLUENCE];
};

#endif