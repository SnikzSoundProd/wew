#pragma once
#include "GameObject.h"
#include <vector>
#include <memory>
#include <string>
#include <algorithm>

class Scene {
public:
    // Хранилище всех объектов на сцене
    std::vector<std::unique_ptr<GameObject>> gameObjects;

    // Метод для создания объекта (только объявление)
    GameObject* createGameObject(const std::string& name = "GameObject");

    // Метод для удаления объекта (только объявление)
    void destroyGameObject(GameObject* go);
};