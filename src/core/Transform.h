#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <nlohmann/json.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../components/Component.h"

using json = nlohmann::json;

struct Transform : public Component {
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    glm::vec3 rotation = {0.0f, 0.0f, 0.0f}; // Углы Эйлера в градусах
    glm::vec3 scale = {1.0f, 1.0f, 1.0f};

    glm::mat4 getModelMatrix() const; // <-- Реализацию вынесем в .cpp

    void serialize(json& jsonData) override;
    void deserialize(const json& jsonData) override;
};