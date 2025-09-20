#pragma once
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class GameObject; // Forward declaration, чтобы избежать циклической зависимости

class Component {
public:
    GameObject* gameObject = nullptr;
    virtual ~Component() = default;

    // --- ВОТ ОН, НАСТОЯЩИЙ МЕТОД ---
    virtual void update(float deltaTime) {}

    // Новые виртуальные методы для сериализации
    virtual void serialize(json& jsonData) {}
    virtual void deserialize(const json& jsonData) {}
};