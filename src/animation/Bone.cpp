#include "Bone.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp> // для mix (lerp)

Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel)
    : m_Name(name), m_ID(ID), m_LocalTransform(1.0f)
{
    m_NumPositions = channel->mNumPositionKeys;
    for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex) {
        aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
        float timeStamp = channel->mPositionKeys[positionIndex].mTime;
        KeyFrame<glm::vec3> data;
        data.value = glm::vec3(aiPosition.x, aiPosition.y, aiPosition.z);
        data.timeStamp = timeStamp;
        m_Positions.push_back(data);
    }

    m_NumRotations = channel->mNumRotationKeys;
    for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex) {
        aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
        float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
        KeyFrame<glm::quat> data;
        data.value = glm::quat(aiOrientation.w, aiOrientation.x, aiOrientation.y, aiOrientation.z);
        data.timeStamp = timeStamp;
        m_Rotations.push_back(data);
    }

    m_NumScalings = channel->mNumScalingKeys;
    for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex) {
        aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
        float timeStamp = channel->mScalingKeys[keyIndex].mTime;
        KeyFrame<glm::vec3> data;
        data.value = glm::vec3(scale.x, scale.y, scale.z);
        data.timeStamp = timeStamp;
        m_Scales.push_back(data);
    }
}

void Bone::update(float animationTime) {
    glm::mat4 translation = interpolatePosition(animationTime);
    glm::mat4 rotation = interpolateRotation(animationTime);
    glm::mat4 scale = interpolateScaling(animationTime);
    m_LocalTransform = translation * rotation * scale;
}

glm::mat4 Bone::getLocalTransform() { return m_LocalTransform; }
std::string Bone::getBoneName() const { return m_Name; }
int Bone::getBoneID() { return m_ID; }

template<typename T>
int Bone::getFrameIndex(float animationTime, const std::vector<KeyFrame<T>>& frames) {
    for (int index = 0; index < frames.size() - 1; ++index) {
        if (animationTime < frames[index + 1].timeStamp)
            return index;
    }
    // Если время вышло за пределы, возвращаем последний валидный индекс
    return frames.size() > 1 ? frames.size() - 2 : 0;
}

glm::mat4 Bone::interpolatePosition(float animationTime) {
    if (m_NumPositions == 1) return glm::translate(glm::mat4(1.0f), m_Positions[0].value);

    int p0Index = getFrameIndex(animationTime, m_Positions);
    int p1Index = p0Index + 1;
    float scaleFactor = (animationTime - m_Positions[p0Index].timeStamp) / (m_Positions[p1Index].timeStamp - m_Positions[p0Index].timeStamp);
    glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].value, m_Positions[p1Index].value, scaleFactor);
    return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::interpolateRotation(float animationTime) {
    if (m_NumRotations == 1) return glm::toMat4(glm::normalize(m_Rotations[0].value));

    int p0Index = getFrameIndex(animationTime, m_Rotations);
    int p1Index = p0Index + 1;
    float scaleFactor = (animationTime - m_Rotations[p0Index].timeStamp) / (m_Rotations[p1Index].timeStamp - m_Rotations[p0Index].timeStamp);
    glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].value, m_Rotations[p1Index].value, scaleFactor);
    return glm::toMat4(glm::normalize(finalRotation));
}

glm::mat4 Bone::interpolateScaling(float animationTime) {
    if (m_NumScalings == 1) return glm::scale(glm::mat4(1.0f), m_Scales[0].value);

    int p0Index = getFrameIndex(animationTime, m_Scales);
    int p1Index = p0Index + 1;
    float scaleFactor = (animationTime - m_Scales[p0Index].timeStamp) / (m_Scales[p1Index].timeStamp - m_Scales[p0Index].timeStamp);
    glm::vec3 finalScale = glm::mix(m_Scales[p0Index].value, m_Scales[p1Index].value, scaleFactor);
    return glm::scale(glm::mat4(1.0f), finalScale);
}