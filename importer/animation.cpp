
#include "animation.h"

#include <functional>
#include "bone.h"
#include "scene.h"


Animation::Animation(const std::string &animationPath, Scene *model) 
{
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
        assert(scene && scene->mRootNode);
        auto animation = scene->mAnimations[0];
        duration_ = animation->mDuration;
        ticks_per_second_ = animation->mTicksPerSecond;
        // aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
        // globalTransformation = globalTransformation.Inverse();
        ReadNodes(m_RootNode, scene->mRootNode);
        ReadBones(animation, *model);
}
    Bone *Animation::FindBone(const std::string &name)
    {
        auto iter = std::find_if(bones_.begin(), bones_.end(),
                                 [&](const Bone &bone) {
                                     return bone.GetBoneName() == name;
                                 }); 
        if (iter == bones_.end())
            return nullptr;
        else
            return &(*iter);
    }

        float Animation::GetTicksPerSecond() { return ticks_per_second_; }
        float Animation::GetDuration() { return duration_; }
        const Node &Animation::GetRootNode() { return m_RootNode; }
        const std::map<std::string, BoneInfo> &Animation::GetBoneIDMap()
    {
        return bone_info_map_;
    }

    void Animation::ReadBones(const aiAnimation *animation, Scene &scene)
    {
        int size = animation->mNumChannels;

        auto &bone_info_map = scene.GetBoneInfoMap();
        int &boneCount = scene.GetBoneCount();

        for (int i = 0; i < size; i++) {
            auto channel = animation->mChannels[i];
            std::string boneName = channel->mNodeName.data;

            bones_.push_back(Bone(boneName, bone_info_map[boneName].id, channel));
        }

        bone_info_map_ = bone_info_map;
    }

    void Animation::ReadNodes(Node &node, const aiNode *src)
    {
        assert(src);

        node.name = src->mName.data;
        node.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
        node.children_count = src->mNumChildren;

        for (int i = 0; i < src->mNumChildren; i++)
        {
            Node new_data;
            ReadNodes(new_data, src->mChildren[i]);
            node.children.push_back(new_data);
        }
    }

