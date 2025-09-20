// --- FULL FIX FOR: Bone.cpp ---
#include "Bone.h"
#include <assimp/anim.h> // Для aiNodeAnim
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp> // для mix (lerp)
#include <glm/gtx/quaternion.hpp>   // для toMat4

Bone::Bone()
	: m_LocalTransform(1.0f), m_ID(-1)
{
}

Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel)
    : m_Name(name), m_ID(ID), m_LocalTransform(1.0f)
{
    for (unsigned int positionIndex = 0; positionIndex < channel->mNumPositionKeys; ++positionIndex) {
        aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
        float timeStamp = channel->mPositionKeys[positionIndex].mTime;
        KeyPosition data;
        data.value = glm::vec3(aiPosition.x, aiPosition.y, aiPosition.z);
        data.timeStamp = timeStamp;
        m_Positions.push_back(data);
    }

    for (unsigned int rotationIndex = 0; rotationIndex < channel->mNumRotationKeys; ++rotationIndex) {
        aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
        float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
        KeyRotation data;
        data.value = glm::quat(aiOrientation.w, aiOrientation.x, aiOrientation.y, aiOrientation.z);
        data.timeStamp = timeStamp;
        m_Rotations.push_back(data);
    }

    for (unsigned int keyIndex = 0; keyIndex < channel->mNumScalingKeys; ++keyIndex) {
        aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
        float timeStamp = channel->mScalingKeys[keyIndex].mTime;
        KeyScale data;
        data.value = glm::vec3(scale.x, scale.y, scale.z);
        data.timeStamp = timeStamp;
        m_Scales.push_back(data);
    }
}

void Bone::Update(float animationTime)
{
    glm::mat4 translation = InterpolatePosition(animationTime);
    glm::mat4 rotation = InterpolateRotation(animationTime);
    glm::mat4 scale = InterpolateScaling(animationTime);
    m_LocalTransform = translation * rotation * scale;
}

int Bone::GetPositionIndex(float animationTime) const
{
    for (size_t index = 0; index < m_Positions.size() - 1; ++index)
    {
        if (animationTime < m_Positions[index + 1].timeStamp) // <-- Используем .timeStamp
            return index;
    }
    return m_Positions.empty() ? 0 : m_Positions.size() - 1;
}

int Bone::GetRotationIndex(float animationTime) const
{
    for (size_t index = 0; index < m_Rotations.size() - 1; ++index)
    {
        if (animationTime < m_Rotations[index + 1].timeStamp) // <-- Используем .timeStamp
            return index;
    }
    return m_Rotations.empty() ? 0 : m_Rotations.size() - 1;
}

int Bone::GetScaleIndex(float animationTime) const
{
    for (size_t index = 0; index < m_Scales.size() - 1; ++index)
    {
        if (animationTime < m_Scales[index + 1].timeStamp) // <-- Используем .timeStamp
            return index;
    }
    return m_Scales.empty() ? 0 : m_Scales.size() - 1;
}

float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const
{
    float scaleFactor = 0.0f;
    float midWayLength = animationTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    if (framesDiff > 0.0f) {
        scaleFactor = midWayLength / framesDiff;
    }
    return scaleFactor;
}

glm::mat4 Bone::InterpolatePosition(float animationTime)
{
    if (m_Positions.size() == 1)
        return glm::translate(glm::mat4(1.0f), m_Positions[0].value); // <-- Используем .value

    int p0Index = GetPositionIndex(animationTime);
    int p1Index = p0Index + 1;
    if (p1Index >= m_Positions.size()) p1Index = p0Index;
    
    float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp, m_Positions[p1Index].timeStamp, animationTime);
    glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].value, m_Positions[p1Index].value, scaleFactor);
    return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::InterpolateRotation(float animationTime)
{
    if (m_Rotations.size() == 1)
    {
        auto rotation = glm::normalize(m_Rotations[0].value); // <-- Используем .value
        return glm::toMat4(rotation);
    }

    int r0Index = GetRotationIndex(animationTime);
    int r1Index = r0Index + 1;
    if (r1Index >= m_Rotations.size()) r1Index = r0Index;

    float scaleFactor = GetScaleFactor(m_Rotations[r0Index].timeStamp, m_Rotations[r1Index].timeStamp, animationTime);
    glm::quat finalRotation = glm::slerp(m_Rotations[r0Index].value, m_Rotations[r1Index].value, scaleFactor);
    finalRotation = glm::normalize(finalRotation);
    return glm::toMat4(finalRotation);
}

glm::mat4 Bone::InterpolateScaling(float animationTime)
{
    if (m_Scales.size() == 1)
        return glm::scale(glm::mat4(1.0f), m_Scales[0].value); // <-- Используем .value

    int s0Index = GetScaleIndex(animationTime);
    int s1Index = s0Index + 1;
    if (s1Index >= m_Scales.size()) s1Index = s0Index;

    float scaleFactor = GetScaleFactor(m_Scales[s0Index].timeStamp, m_Scales[s1Index].timeStamp, animationTime);
    glm::vec3 finalScale = glm::mix(m_Scales[s0Index].value, m_Scales[s1Index].value, scaleFactor);
    return glm::scale(glm::mat4(1.0f), finalScale);
}