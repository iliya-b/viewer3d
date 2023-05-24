#ifndef ANIMATION_H_
#define ANIMATION_H_
#include <functional>
#include "bone.h"
#include "scene.h"

struct Node
{
    glm::mat4 transformation;
    std::string name;
    int children_count;
    std::vector<Node> children;
};

class Animation
{
public:
    Animation() = default;

    Animation(const std::string &animationPath, Scene *model);

    ~Animation() = default;
    Bone *FindBone(const std::string &name);

    float GetTicksPerSecond();
    float GetDuration();
    const Node &GetRootNode();
    const std::map<std::string, BoneInfo> &GetBoneIDMap();

private:
    void ReadBones(const aiAnimation *animation, Scene &model);
    void ReadNodes(Node &dest, const aiNode *src);
    float duration_;
    int ticks_per_second_;
    std::vector<Bone> bones_;
    Node m_RootNode;
    std::map<std::string, BoneInfo> bone_info_map_;
};
#endif