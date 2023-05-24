#ifndef ANIMATOR_H_
#define ANIMATOR_H_
#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "animation.h"
#include "bone.h"

class Animator
{
public:
    Animator(Animation *animation);

    void UpdateAnimation(float dt);

    void PlayAnimation(Animation *pAnimation);
    void CalculateTransform(const Node *node, glm::mat4 parentTransform);
    std::vector<glm::mat4> GetTransforms();

private:
    std::vector<glm::mat4> final_bone_matrices_;
    Animation *current_animation_;
    float current_time_;
    float delta_time_;
};

#endif