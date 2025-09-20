#pragma once
#include "Component.h"
#include <glm/glm.hpp>

struct LightComponent : public Component {
    glm::vec3 color = {1.0f, 1.0f, 1.0f}; // Белый свет по умолчанию
    float intensity = 1.0f;

    // Поля для анимации вращения
    bool rotate = false;
    float rotationSpeed = 10.0f; // Градусов в секунду
    float rotationRadius = 10.0f;
    glm::vec3 rotationCenter = {0.0f, 0.0f, -5.0f};

    // Переопределяем update для анимации
    void update(float deltaTime) override {
        if (rotate && gameObject) {
            // Получаем текущий угол
            float currentAngle = glm::radians(fmod(glfwGetTime() * rotationSpeed, 360.0));
            
            // Вычисляем новую позицию на окружности
            float x = rotationCenter.x + rotationRadius * cos(currentAngle);
            float z = rotationCenter.z + rotationRadius * sin(currentAngle);
            
            // Обновляем transform нашего источника света
            gameObject->transform->position.x = x;
            gameObject->transform->position.y = rotationCenter.y + 2.0f; 
            gameObject->transform->position.z = z;
        }
    }

    // Включаем сюда GLFW для glfwGetTime()
    // Это не самое чистое решение, но для начала сойдет
    #include <GLFW/glfw3.h>
};