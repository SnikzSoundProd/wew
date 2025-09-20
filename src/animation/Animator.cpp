// --- FULL FIX FOR: Animator.cpp ---
#include "Animator.h"

Animator::Animator(Animation* animation) {
    m_CurrentTime = 0.0;
    m_CurrentAnimation = animation;

    m_FinalBoneMatrices.reserve(100);
    for (int i = 0; i < 100; i++)
        m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
}

void Animator::updateAnimation(float dt) {
    m_DeltaTime = dt;
    if (m_CurrentAnimation) {
        // --- ИСПОЛЬЗУЕМ ПРАВИЛЬНЫЕ ГЕТТЕРЫ ---
        m_CurrentTime += m_CurrentAnimation->getTicksPerSecond() * dt;
        m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->getDuration());
        calculateBoneTransform(&m_CurrentAnimation->getRootNode(), glm::mat4(1.0f));
    }
}

void Animator::playAnimation(Animation* pAnimation) {
    m_CurrentAnimation = pAnimation;
    m_CurrentTime = 0.0f;
}

void Animator::calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform) {
    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;

    Bone* bone = m_CurrentAnimation->findBone(nodeName);

    if (bone) {
        bone->Update(m_CurrentTime); // <-- Вызываем Update с большой буквы
        nodeTransform = bone->getLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    const auto& boneInfoMap = m_CurrentAnimation->getBoneIDMap(); // <-- используем const&
    if (boneInfoMap.count(nodeName)) { // <-- .count() безопаснее
        int index = boneInfoMap.at(nodeName).id;
        glm::mat4 offset = boneInfoMap.at(nodeName).offset;
        m_FinalBoneMatrices[index] = globalTransformation * offset;
    }

    for (size_t i = 0; i < node->children.size(); i++) // <-- Безопаснее через .size()
        calculateBoneTransform(&node->children[i], globalTransformation);
}

std::vector<glm::mat4> Animator::getFinalBoneMatrices() {
    return m_FinalBoneMatrices;
}