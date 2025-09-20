#include "SceneSerializer.h"
#include <fstream>
#include "GameObject.h"
#include "Transform.h"
#include "../components/MeshRenderer.h"

SceneSerializer::SceneSerializer(Scene& scene) : m_Scene(scene) {}

void SceneSerializer::serialize(const std::string& filepath) {
    json sceneJson;
    json gameObjectsJson = json::array();

    for (const auto& go : m_Scene.gameObjects) {
        json goJson;
        goJson["name"] = go->name;
        
        json componentsJson = json::array();
        for (const auto& comp : go->getComponents()) {
            json compJson;
            if (auto* transform = dynamic_cast<Transform*>(comp.get())) {
                compJson["type"] = "Transform";
                transform->serialize(compJson);
            } else if (auto* renderer = dynamic_cast<MeshRenderer*>(comp.get())) {
                compJson["type"] = "MeshRenderer";
                renderer->serialize(compJson);
            }
            componentsJson.push_back(compJson);
        }
        goJson["components"] = componentsJson;
        gameObjectsJson.push_back(goJson);
    }
    sceneJson["gameObjects"] = gameObjectsJson;

    std::ofstream o(filepath);
    o << std::setw(4) << sceneJson << std::endl;
}

bool SceneSerializer::deserialize(const std::string& filepath) {
    std::ifstream i(filepath);
    if (!i.is_open()) return false;

    json sceneJson;
    i >> sceneJson;

    // Очищаем текущую сцену
    m_Scene.gameObjects.clear();

    for (const auto& goJson : sceneJson["gameObjects"]) {
        GameObject* go = m_Scene.createGameObject(goJson["name"]);

        for (const auto& compJson : goJson["components"]) {
            std::string type = compJson["type"];
            if (type == "Transform") {
                go->transform->deserialize(compJson);
            } else if (type == "MeshRenderer") {
                MeshRenderer* renderer = go->addComponent<MeshRenderer>();
                renderer->deserialize(compJson);
            }
        }
    }
    return true;
}