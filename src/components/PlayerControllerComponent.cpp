#include "PlayerControllerComponent.h"
#include "../core/GameObject.h"
#include "../core/PhysicsEngine.h"
#include "RigidbodyComponent.h"
#include "AnimationComponent.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <GLFW/glfw3.h>
#include <iostream> // Для отладки

void PlayerControllerComponent::init() {
    m_rigidbody = gameObject->getComponent<RigidbodyComponent>();
    m_animComp = gameObject->getComponent<AnimationComponent>();
    m_window = glfwGetCurrentContext();

    m_targetRotation = glm::quat(glm::radians(gameObject->transform->rotation));
    
    // Отладка
    if (!m_window) {
        std::cout << "WARNING: PlayerController не смог получить окно!" << std::endl;
    }
    if (!m_rigidbody) {
        std::cout << "WARNING: PlayerController не нашел RigidbodyComponent!" << std::endl;
    }
}

void PlayerControllerComponent::init(GLFWwindow* window) {
    m_rigidbody = gameObject->getComponent<RigidbodyComponent>();
    m_window = window;
    
    std::cout << "PlayerController инициализирован с окном: " << (m_window ? "OK" : "FAIL") << std::endl;
}

// Добавим новый метод для установки камеры
void PlayerControllerComponent::setCamera(Camera* camera) {
    m_camera = camera;
}

void PlayerControllerComponent::update(float deltaTime) {
    if (!m_rigidbody || !m_window) return;
    
    btRigidBody* body = m_rigidbody->getBtRigidbody();
    if (!body) return;

    body->activate(true);

    // === ДВИЖЕНИЕ С РАЗУМНЫМИ СИЛАМИ ===
    btVector3 force(0, 0, 0);
    float forceAmount = 10.0f; // БЫЛО 500, СТАЛО 10!

    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) force += btVector3(0, 0, -1);
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) force += btVector3(0, 0, 1);
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) force += btVector3(-1, 0, 0);
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) force += btVector3(1, 0, 0);

    if (force.length() > 0.1f) {
        force.normalize();
        body->applyCentralForce(force * forceAmount);
    }

    if (m_animComp) {
        if (force.length() > 0.1) { // Если мы движемся
            m_animComp->play("run");
        } else { // Если стоим на месте
            m_animComp->play("idle");
        }
    }
    
    // === ОГРАНИЧЕНИЕ СКОРОСТИ ===
    btVector3 velocity = body->getLinearVelocity();
    float maxSpeed = 20.0f; // Максимальная скорость
    
    // Ограничиваем горизонтальную скорость (X и Z)
    btVector3 horizontalVel(velocity.getX(), 0, velocity.getZ());
    if (horizontalVel.length() > maxSpeed) {
        horizontalVel.normalize();
        horizontalVel *= maxSpeed;
        body->setLinearVelocity(btVector3(horizontalVel.getX(), velocity.getY(), horizontalVel.getZ()));
    }

    // --- НОВЫЙ БЛОК: ВРАЩЕНИЕ ПЕРСОНАЖА ---
    if (force.length() > 0.1f) {
        // 1. Вычисляем целевой угол
        // Atan2 - магическая функция, которая правильно вычисляет угол по вектору
        float targetAngle = atan2(force.getX(), force.getZ());
        // 2. Создаем кватернион для этого угла (вокруг оси Y)
        m_targetRotation = glm::angleAxis(targetAngle, glm::vec3(0, 1, 0));
    }
    
    // 3. Плавно интерполируем текущий поворот к целевому
    glm::quat currentRotation = glm::quat(glm::radians(gameObject->transform->rotation));
    gameObject->transform->rotation = glm::degrees(glm::eulerAngles(glm::slerp(currentRotation, m_targetRotation, deltaTime * rotationSpeed)));
    // === ПРЫЖОК ===
    if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        float currentVelocityY = body->getLinearVelocity().getY();
        if (std::abs(currentVelocityY) < 1.0f) {
            body->applyCentralImpulse(btVector3(0, jumpForce, 0));
            std::cout << "ПРЫЖОК!" << std::endl;
        }
    }

     // --- НОВЫЙ БЛОК: СТРЕЛЬБА ---
    // glfwGetMouseButton проверяет клик, а не зажатие
    if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (m_camera) {
            // 1. Выпускаем луч из позиции камеры в направлении ее взгляда
            glm::vec3 rayFrom = m_camera->position;
            glm::vec3 rayTo = rayFrom + m_camera->front * 100.0f; // Луч длиной 100 метров

            // 2. Делаем рейкаст
            RaycastResult result = PhysicsEngine::GetInstance().raycast(rayFrom, rayTo);

            // 3. Если во что-то попали, и это не статичный пол
            if (result.hasHit && result.body && result.body->getMass() > 0.0f) {
                // 4. "Пинаем" этот объект!
                btVector3 impulse = btVector3(m_camera->front.x, m_camera->front.y, m_camera->front.z);
                impulse.normalize();
                result.body->applyCentralImpulse(impulse * shootImpulse);
            }
        }
    }
    
    // Отладка (каждые 30 кадров)
    static int debugCounter = 0;
    if (debugCounter % 30 == 0) {
        btVector3 pos = body->getWorldTransform().getOrigin();
        btVector3 vel = body->getLinearVelocity();
        std::cout << "Позиция: " << pos.x() << ", " << pos.y() << ", " << pos.z() 
                  << " | Скорость: " << vel.x() << ", " << vel.y() << ", " << vel.z() << std::endl;
    }
    debugCounter++;
}