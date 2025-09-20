#pragma once
#include "AnimData.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

class Bone {
public:
    Bone(const std::string& name, int ID, const aiNodeAnim* channel);
    void update(float animationTime);
    glm::mat4 getLocalTransform();
    std::string getBoneName() const;
    int getBoneID();
private:
    template<typename T>
    int getFrameIndex(float animationTime, const std::vector<KeyFrame<T>>& frames);
    glm::mat4 interpolatePosition(float animationTime);
    glm::mat4 interpolateRotation(float animationTime);
    glm::mat4 interpolateScaling(float animationTime);
    std::vector<KeyFrame<glm::vec3>> m_Positions;
    std::vector<KeyFrame<glm::quat>> m_Rotations;
    std::vector<KeyFrame<glm::vec3>> m_Scales;
    int m_NumPositions;
    int m_NumRotations;
    int m_NumScalings;
    glm::mat4 m_LocalTransform;
    std::string m_Name;
    int m_ID;
};