#include "animator.h"

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "animation.h"
#include "bone.h"
#include "glm/gtx/string_cast.hpp"

Animator::Animator(Animation *animation)
    {
        current_time_ = 0.0;
        current_animation_ = animation;

        final_bone_matrices_.reserve(100);

        for (int i = 0; i < 100; i++)
            final_bone_matrices_.push_back(glm::mat4(1.0f));
    }

    void Animator::UpdateAnimation(float dt)
    {
        delta_time_ = dt;
        current_time_ += current_animation_->GetTicksPerSecond() * dt;
        current_time_ = fmod(current_time_, current_animation_->GetDuration());
        CalculateTransform(&current_animation_->GetRootNode(), glm::mat4(1.0f));
        
    }

    void Animator::PlayAnimation(Animation *pAnimation)
    {
        current_animation_ = pAnimation;
        current_time_ = 0.0f;
    }

    void Animator::CalculateTransform(const Node *node, glm::mat4 parent_transform)
    {
        std::string node_name = node->name;
        glm::mat4 node_transform = node->transformation;

        Bone *bone = current_animation_->FindBone(node_name);

        if (bone)  {
            bone->Update(current_time_);
            node_transform = bone->GetLocalTransform();
        }

        glm::mat4 globalTransformation = parent_transform * node_transform;

        auto bone_info_map = current_animation_->GetBoneIDMap();
        if (bone_info_map.find(node_name) != bone_info_map.end()) {
            int index = bone_info_map[node_name].id;
            glm::mat4 offset = bone_info_map[node_name].offset;
            final_bone_matrices_[index] = globalTransformation * offset;
        }

        for (int i = 0; i < node->children_count; i++)
            CalculateTransform(&node->children[i], globalTransformation);
    }

    std::vector<glm::mat4> Animator::GetTransforms()
    {
        return final_bone_matrices_;
    }
