#include "Animation.h"
#include <algorithm>

Animation::Animation(const std::string& animationPath, Model* model) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
    assert(scene && scene->mRootNode);

    auto animation = scene->mAnimations[0];
    m_Duration = animation->mDuration;
    m_TicksPerSecond = animation->mTicksPerSecond;

    readHierarchyData(m_RootNode, scene->mRootNode);
    readMissingBones(animation, *model);
}

Bone* Animation::findBone(const std::string& name) {
    auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
        [&](const Bone& bone) {
            return bone.getBoneName() == name;
        }
    );
    if (iter == m_Bones.end()) return nullptr;
    else return &(*iter);
}

void Animation::readMissingBones(const aiAnimation* animation, Model& model) {
    int size = animation->mNumChannels;
    
    // ИСПРАВЛЕНИЕ: правильный доступ к методам Model
    auto& boneInfoMap = model.getBoneInfoMap(); // БЕЗ ->
    int& boneCount = model.getBoneCount();       // БЕЗ ->

    for (int i = 0; i < size; i++) {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
            boneInfoMap[boneName].id = boneCount;
            boneCount++;
        }
        m_Bones.push_back(Bone(boneName, boneInfoMap[boneName].id, channel));
    }
    
    // ИСПРАВЛЕНИЕ: правильное присвоение
    m_BoneInfoMap = boneInfoMap;
}

void Animation::readHierarchyData(AssimpNodeData& dest, const aiNode* src) {
    assert(src);
    dest.name = src->mName.data;
    dest.transformation = ConvertMatrixToGLMFormat(src->mTransformation);
    dest.childrenCount = src->mNumChildren;
    dest.children.reserve(src->mNumChildren);

    for (unsigned int i = 0; i < src->mNumChildren; i++) {
        AssimpNodeData newData;
        readHierarchyData(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
}