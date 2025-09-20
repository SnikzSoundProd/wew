#pragma once
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include "Transform.h"
#include "../components/Component.h"

class GameObject {
public:
    Transform* transform;
    std::string name = "GameObject";

    GameObject() {
        transform = addComponent<Transform>();
    }

    template<typename T, typename... Args>
    T* addComponent(Args&&... args) {
        auto newComponent = std::make_unique<T>(std::forward<Args>(args)...);
        newComponent->gameObject = this;
        T* rawPtr = newComponent.get();
        m_components.push_back(std::move(newComponent));
        return rawPtr;
    }

    template<typename T>
    T* getComponent() {
        for (const auto& component : m_components) {
            if (auto casted = dynamic_cast<T*>(component.get())) {
                return casted;
            }
        }
        return nullptr;
    }
    
    const std::vector<std::unique_ptr<Component>>& getComponents() const {
        return m_components;
    }

private:
    std::vector<std::unique_ptr<Component>> m_components;
};