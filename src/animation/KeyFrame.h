// --- CREATE NEW FILE KeyFrame.h ---
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

template<typename T>
struct KeyFrame
{
    T value;
    float timeStamp;
};

// Можно сразу определить псевдонимы для удобства
using KeyPosition = KeyFrame<glm::vec3>;
using KeyRotation = KeyFrame<glm::quat>;
using KeyScale = KeyFrame<glm::vec3>;