#include "scene.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <numeric>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "vertex.h"
#include "texture.h"
#include "mesh.h"
#include <range/v3/range.hpp>
#include <range/v3/view/counted.hpp>
#include <range/v3/view/zip.hpp>
#include <range/v3/view/span.hpp>
#include <range/v3/view/for_each.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/action/join.hpp>

using namespace ranges;

unsigned int UploadTexture(const aiScene *scene, const char *path, const std::string &directory)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;

    const aiTexture *texture = scene->GetEmbeddedTexture(path);
    const char *hint = texture->achFormatHint;

    unsigned char *data = stbi_load_from_memory((unsigned char *)texture->pcData, texture->mWidth, &width, &height, &nrComponents, 4);
    const char *err = stbi_failure_reason();

    if (texture)
    {
        GLenum format = GL_RGBA;
        // GLenum internal_format = GL_COMPRESSED_RGBA;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);

        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << " " << filename << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}


std::vector<Texture> Scene::loadMaterialTextures(const aiScene *scene, aiMaterial *mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString texturePath;
        mat->GetTexture(type, i, &texturePath);
        Texture texture;
        texture.id = UploadTexture(scene, texturePath.C_Str(), this->directory);
        texture.type = typeName;
        texture.path = texturePath.C_Str();
        textures.push_back(texture);
    }
    return textures;
}



void SetVertexBoneData(Vertex &vertex, int boneID, float weight)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
    {
        if (vertex.bone_id[i] < 0)
        {
            vertex.weights[i] = weight;
            vertex.bone_id[i] = boneID;
            break;
        }
    }
}

void Scene::SetVertexBoneDataToDefault(Vertex &vertex)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        vertex.bone_id[i] = -1;
        vertex.weights[i] = 0.0f;
    }
}

void Scene::ExtractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh *mesh, const aiScene *scene)
{
    for (auto bone : ranges::span(mesh->mBones, mesh->mNumBones))
    {
        int bone_id = -1;
        std::string name = bone->mName.C_Str();
        if (bone_info_map_.find(name) == bone_info_map_.end()) {
            BoneInfo newBoneInfo;
            newBoneInfo.id = bone_counter_;
            newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(bone->mOffsetMatrix);
            bone_info_map_[name] = newBoneInfo;
            bone_id = bone_counter_;
            bone_counter_++;
        } else   {
            bone_id = bone_info_map_[name].id;
        }
        assert(bone_id != -1);
        
        for (auto weight_info : ranges::span(bone->mWeights, bone->mNumWeights))
        {
            int vertex_id = weight_info.mVertexId;
            float weight = weight_info.mWeight;
            assert(vertex_id <= vertices.size());
            SetVertexBoneData(vertices[vertex_id], bone_id, weight);
        }
    }
}

Scene::Scene(std::string const &path)
{
    stbi_set_flip_vertically_on_load(true);

    this->load(path);
};

void Scene::load(std::string const &path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

    assert(scene && scene->mRootNode);


    directory = path.substr(0, path.find_last_of('/'));

    for(auto mesh: ranges::span(scene->mMeshes, scene->mNumMeshes)){
        meshes.push_back(processMesh(mesh, scene));
    }
};


void Scene::Draw(Shader &shader)
{
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].Draw(shader);
    }
}



Mesh Scene::processMesh(aiMesh *mesh, const aiScene *scene)
{
    auto mVertices      = ranges::span(mesh->mVertices, mesh->mNumVertices);
    auto mNormals       = ranges::span(mesh->mNormals, mesh->mNumVertices);
    auto mTextureCoords = ranges::span(mesh->mTextureCoords[0], mesh->mNumVertices);
    auto mFaces         = ranges::span(mesh->mFaces, mesh->mNumFaces);

    auto _vertices =
        ranges::views::zip_with(
            [](auto v, auto n, auto t)
            {
                Vertex res = {glm::vec3(v.x, v.y, v.z), glm::vec3(n.x, n.y, n.z), glm::vec2(t.x, t.y)};
                for (int i = 0; i < MAX_BONE_INFLUENCE; i++){
                    res.bone_id[i] = -1;
                    res.weights[i] = 0.0f;
                }
                return res;
            },
            mVertices, mNormals, mTextureCoords);
    auto vertices = std::vector<Vertex>(_vertices.begin(), _vertices.end());

    std::vector<unsigned int> indices;
    for (auto face: mFaces){
        for(auto idx: ranges::span(face.mIndices, face.mNumIndices)){
            indices.push_back(idx);
        }
    }

    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    auto textures = actions::join(
        std::vector<std::vector<Texture>>{
            loadMaterialTextures(scene, material, aiTextureType_DIFFUSE, "diffuse"),
            loadMaterialTextures(scene, material, aiTextureType_SPECULAR, "specular"),
            loadMaterialTextures(scene, material, aiTextureType_HEIGHT, "height"),
            loadMaterialTextures(scene, material, aiTextureType_AMBIENT, "ambient")
        }
    ) | ranges::to<std::vector<Texture>>;

    ExtractBoneWeightForVertices(vertices, mesh, scene);

    std::cout << "vertices " << sizeof(vertices) / 1024 / 1024 << " mb"  << std::endl;
    std::cout << "textures " << sizeof(textures) / 1024 / 1024 << " mb"  << std::endl;

    return Mesh(vertices, indices, textures);
}

    std::map<std::string, BoneInfo> &Scene::GetBoneInfoMap() { return bone_info_map_; }
    int &Scene::GetBoneCount() { return bone_counter_; }
