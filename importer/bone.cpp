#include "bone.h"
#include <vector>
#include <assimp/scene.h>
#include <list>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "assimp_glm_helpers.h"
#include <iostream>


    Bone::Bone(const std::string &name, int ID, const aiNodeAnim *channel)
        : name_(name),
          id_(ID),
          local_transform_(1.0f)
    {
        positions_count_ = channel->mNumPositionKeys;

        for (int positionIndex = 0; positionIndex < positions_count_; ++positionIndex)
        {
            aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
            float timeStamp = channel->mPositionKeys[positionIndex].mTime;
            KeyPosition data;
            data.position = AssimpGLMHelpers::GetGLMVec(aiPosition);
            data.timeStamp = timeStamp;
            positions_.push_back(data);
        }

        rotations_count_ = channel->mNumRotationKeys;
        for (int rotationIndex = 0; rotationIndex < rotations_count_; ++rotationIndex)
        {
            aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
            float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
            KeyRotation data;
            data.orientation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
            data.timeStamp = timeStamp;
            rotations_.push_back(data);
        }

        scalings_count_ = channel->mNumScalingKeys;
        for (int keyIndex = 0; keyIndex < scalings_count_; ++keyIndex)
        {
            aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
            float timeStamp = channel->mScalingKeys[keyIndex].mTime;
            KeyScale data;
            data.scale = AssimpGLMHelpers::GetGLMVec(scale);
            data.timeStamp = timeStamp;
            scales_.push_back(data);
        }
    }

    void Bone::Update(float animationTime)
    {
        glm::mat4 translation = InterpolatePosition(animationTime);
        glm::mat4 rotation = InterpolateRotation(animationTime);
        glm::mat4 scale = InterpolateScaling(animationTime);
        local_transform_ = translation * rotation * scale;
    }

    glm::mat4 Bone::GetLocalTransform() { return local_transform_; }
    const std::string& Bone::GetBoneName() const { return name_; }
    int Bone::GetBoneID() { return id_; }

    int Bone::GetPositionIndex(float animationTime)
    {
        for (int index = 0; index < positions_count_ - 1; ++index)
        {
            if (animationTime < positions_[index + 1].timeStamp)
                return index;
        }
        assert(0);
    }

    int Bone::GetRotationIndex(float animationTime)
    {
        for (int index = 0; index < rotations_count_ - 1; ++index)
        {
            if (animationTime < rotations_[index + 1].timeStamp)
                return index;
        }
        assert(0);
    }

    int Bone::GetScaleIndex(float animationTime)
    {
        for (int index = 0; index < scalings_count_ - 1; ++index)
        {
            if (animationTime < scales_[index + 1].timeStamp)
                return index;
        }
        assert(0);
    }
 
    float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
    {
        float scale_factor = 0.0f;
        float midWayLength = animationTime - lastTimeStamp;
        float framesDiff = nextTimeStamp - lastTimeStamp;
        scale_factor = midWayLength / framesDiff;
        return scale_factor;
    }

    glm::mat4 Bone::InterpolatePosition(float animationTime)
    {
        if (1 == positions_count_)
            return glm::translate(glm::mat4(1.0f), positions_[0].position);

        int p0Index = GetPositionIndex(animationTime);
        int p1Index = p0Index + 1;
        float scale_factor = GetScaleFactor(positions_[p0Index].timeStamp,
                                           positions_[p1Index].timeStamp, animationTime);
        glm::vec3 finalPosition = glm::mix(positions_[p0Index].position, positions_[p1Index].position, scale_factor);
        return glm::translate(glm::mat4(1.0f), finalPosition);
    }

    glm::mat4 Bone::InterpolateRotation(float animationTime)
    {
        if ( 1 == rotations_count_)
        {
            auto rotation = glm::normalize(rotations_[0].orientation);
            return glm::toMat4(rotation);
        }

        int p0Index = GetRotationIndex(animationTime);
        int p1Index = p0Index + 1;
        float scale_factor = GetScaleFactor(rotations_[p0Index].timeStamp,
                                           rotations_[p1Index].timeStamp, animationTime);
        glm::quat finalRotation = glm::slerp(rotations_[p0Index].orientation, rotations_[p1Index].orientation, scale_factor);
        finalRotation = glm::normalize(finalRotation);
        return glm::toMat4(finalRotation);
    }

    glm::mat4 Bone::InterpolateScaling(float animationTime)
    {
        if (1 == scalings_count_)
            return glm::scale(glm::mat4(1.0f), scales_[0].scale);

        int p0Index = GetScaleIndex(animationTime);
        int p1Index = p0Index + 1;
        float scale_factor = GetScaleFactor(scales_[p0Index].timeStamp,
                                           scales_[p1Index].timeStamp, animationTime);
        glm::vec3 finalScale = glm::mix(scales_[p0Index].scale, scales_[p1Index].scale, scale_factor);
        return glm::scale(glm::mat4(1.0f), finalScale);
    }
    