#pragma once
#include "Component.h"
#include <GLFW/glfw3.h> // Нам нужен доступ к GLFW для проверки клавиш

// Forward declaration, чтобы избежать циклической зависимости
class RigidbodyComponent;
class AnimationComponent;

class PlayerControllerComponent : public Component {
public:
    float moveSpeed = 5.0f;
    float jumpForce = 5.0f;
    
    PlayerControllerComponent() = default;

    void init(); // Для поиска Rigidbody
    void init(GLFWwindow* window);
    void update(float deltaTime) override;

private:
    RigidbodyComponent* m_rigidbody = nullptr;
    AnimationComponent* m_animComp = nullptr;
    GLFWwindow* m_window = nullptr; // Нам нужно окно, чтобы проверять нажатия
};