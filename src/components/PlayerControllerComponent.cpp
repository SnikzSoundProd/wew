#include "PlayerControllerComponent.h"
#include "../core/GameObject.h"
#include "../graphics/Camera.h"
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

     // --- 1. ОПРЕДЕЛЯЕМ НАПРАВЛЕНИЕ ВВОДА ---
    glm::vec3 moveDirection(0.0f);
    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) moveDirection += glm::vec3(0, 0, 1);
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) moveDirection += glm::vec3(0, 0, -1);
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) moveDirection += glm::vec3(1, 0, 0);
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) moveDirection += glm::vec3(-1, 0, 0);
    if (glm::length(moveDirection) > 0.1f) {
        moveDirection = glm::normalize(moveDirection);
    }

    // --- 2. ВРАЩЕНИЕ ПЕРСОНАЖА И СИЛЫ ---
    // Получаем горизонтальный поворот камеры
    glm::quat cameraYaw = glm::angleAxis(glm::radians(m_camera->yaw), glm::vec3(0, 1, 0));
    
    // Поворачиваем вектор движения в соответствии с камерой
    glm::vec3 rotatedMoveDirection = cameraYaw * moveDirection;
    
    btVector3 force(0, 0, 0);
    if (glm::length(rotatedMoveDirection) > 0.1f) {
        // Прикладываем силу в направлении, куда смотрит камера
        force = btVector3(rotatedMoveDirection.x, 0, rotatedMoveDirection.z);
        if(force.length() > 0.1f) force.normalize();

        // Поворачиваем персонажа в сторону движения
        float targetAngle = atan2(force.getX(), force.getZ());
        m_targetRotation = glm::angleAxis(targetAngle, glm::vec3(0, 1, 0));
    }

    // ТВОЙ ТЮНИНГ СИЛЫ!
    float forceAmount = 10.0f;
    body->applyCentralForce(force * forceAmount);

    // Плавно поворачиваем
    glm::quat currentRotation = glm::quat(glm::radians(gameObject->transform->rotation));
    gameObject->transform->rotation = glm::degrees(glm::eulerAngles(glm::slerp(currentRotation, m_targetRotation, deltaTime * rotationSpeed)));

    // --- ТВОЙ КОД ОГРАНИЧЕНИЯ СКОРОСТИ ---
    btVector3 velocity = body->getLinearVelocity();
    float maxSpeed = 20.0f;
    btVector3 horizontalVel(velocity.getX(), 0, velocity.getZ());
    if (horizontalVel.length() > maxSpeed) {
        horizontalVel.normalize();
        horizontalVel *= maxSpeed;
        body->setLinearVelocity(btVector3(horizontalVel.getX(), velocity.getY(), horizontalVel.getZ()));
    }

    // --- АНИМАЦИЯ ---
    if (m_animComp) {
        if (force.length() > 0.1f) m_animComp->play("run");
        else m_animComp->play("idle");
    }

    // --- ПРЫЖОК ---
    if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (std::abs(body->getLinearVelocity().getY()) < 1.0f) {
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