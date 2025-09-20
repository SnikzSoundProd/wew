#include "Transform.h"

glm::mat4 Transform::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), {1.0f, 0.0f, 0.0f});
    model = glm::rotate(model, glm::radians(rotation.y), {0.0f, 1.0f, 0.0f});
    model = glm::rotate(model, glm::radians(rotation.z), {0.0f, 0.0f, 1.0f});
    model = glm::scale(model, scale);
    return model;
}

void Transform::serialize(json& jsonData) {
    jsonData["position"] = {position.x, position.y, position.z};
    jsonData["rotation"] = {rotation.x, rotation.y, rotation.z};
    jsonData["scale"] = {scale.x, scale.y, scale.z};
}

void Transform::deserialize(const json& jsonData) {
    position = {jsonData["position"][0], jsonData["position"][1], jsonData["position"][2]};
    rotation = {jsonData["rotation"][0], jsonData["rotation"][1], jsonData["rotation"][2]};
    scale = {jsonData["scale"][0], jsonData["scale"][1], jsonData["scale"][2]};
}