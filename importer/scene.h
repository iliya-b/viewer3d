#ifndef MODEL_H_
#define MODEL_H_

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "texture.h"
#include "mesh.h"
#include "shader.h"
#include "bone.h"

class Scene
{

public:
    std::vector<Mesh> meshes;
    std::string directory;

    Scene(std::string const &path);
    void Draw(Shader &shader);

    std::map<std::string, BoneInfo> &GetBoneInfoMap();
    int &GetBoneCount();

private:
    std::map<std::string, BoneInfo> bone_info_map_;
    int bone_counter_ = 0;

    void load(std::string const &path);
    void SetVertexBoneDataToDefault(Vertex &vertex);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    std::vector<Texture> loadMaterialTextures(const aiScene *scene, aiMaterial *mat, aiTextureType type, std::string typeName);
    void ExtractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh *mesh, const aiScene *scene);
};

#endif