#include "Scene.h"

// Реализация метода createGameObject
GameObject* Scene::createGameObject(const std::string& name) {
    auto go = std::make_unique<GameObject>();
    go->name = name;
    GameObject* ptr = go.get();
    gameObjects.push_back(std::move(go));
    return ptr;
}

// Реализация метода destroyGameObject
void Scene::destroyGameObject(GameObject* go) {
    // Находим и удаляем объект из вектора
    gameObjects.erase(
        std::remove_if(gameObjects.begin(), gameObjects.end(), 
            [&](const std::unique_ptr<GameObject>& obj) {
                return obj.get() == go;
            }),
        gameObjects.end()
    );
}