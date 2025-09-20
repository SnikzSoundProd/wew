#pragma once
#include "AnimData.h"
#include "../graphics/Model.h" // Для доступа к m_BoneInfoMap
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct AssimpNodeData {
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;
};

class Animation {
public:
    Animation() = default;
    Animation(const std::string& animationPath, Model* model);
    ~Animation() = default;

    Bone* findBone(const std::string& name);
    
    float getTicksPerSecond() { return m_TicksPerSecond; }
    float getDuration() { return m_Duration; }
    const AssimpNodeData& getRootNode() { return m_RootNode; }
    const std::map<std::string, BoneInfo>& getBoneIDMap() { return m_BoneInfoMap; }

private:
    void readMissingBones(const aiAnimation* animation, Model& model);
    void readHierarchyData(AssimpNodeData& dest, const aiNode* src);

    float m_Duration;
    int m_TicksPerSecond;
    std::vector<Bone> m_Bones;
    AssimpNodeData m_RootNode;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
};