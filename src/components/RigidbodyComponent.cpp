#include "RigidbodyComponent.h"
#include "../core/GameObject.h"
#include "../core/PhysicsEngine.h"
#include "ColliderComponent.h"
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>

RigidbodyComponent::RigidbodyComponent() {}
RigidbodyComponent::~RigidbodyComponent() {
    if (m_rigidbody) {
        PhysicsEngine::GetInstance().getDynamicsWorld()->removeRigidBody(m_rigidbody);
        delete m_rigidbody;
    }
    delete m_motionState;
}


void RigidbodyComponent::init() {
    ColliderComponent* collider = gameObject->getComponent<ColliderComponent>();
    if (!collider) {
        std::cout << "ERROR: RigidbodyComponent не нашел ColliderComponent!" << std::endl;
        return;
    }

    btCollisionShape* shape = collider->createShape();
    shape->setLocalScaling(btVector3(
        gameObject->transform->scale.x, 
        gameObject->transform->scale.y, 
        gameObject->transform->scale.z
    ));

    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(
        gameObject->transform->position.x, 
        gameObject->transform->position.y, 
        gameObject->transform->position.z
    ));
    
    m_motionState = new btDefaultMotionState(startTransform);

    btVector3 localInertia(0, 0, 0);
    if (mass != 0.0f) {
        shape->calculateLocalInertia(mass, localInertia);
    }

    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, m_motionState, shape, localInertia);
    m_rigidbody = new btRigidBody(rbInfo);
    
    // === УЛУЧШЕННЫЕ НАСТРОЙКИ ФИЗИКИ ===
    if (mass > 0.0f) {
        // Увеличиваем затухание, чтобы объект не летал бесконечно
        m_rigidbody->setDamping(0.8f, 0.8f); // БЫЛО 0.2f, СТАЛО 0.8f
        
        // Увеличиваем трение
        m_rigidbody->setFriction(0.8f); // БЫЛО 0.3f, СТАЛО 0.8f
        
        // Добавляем сопротивление воздуха
        m_rigidbody->setSleepingThresholds(0.8f, 1.0f);
        
        // Не даем объекту засыпать слишком быстро
        m_rigidbody->setActivationState(DISABLE_DEACTIVATION);
        
        std::cout << "Настройки физики: damping=0.8, friction=0.8" << std::endl;
    }
    
    PhysicsEngine::GetInstance().addRigidbody(m_rigidbody);
    
    std::cout << "RigidbodyComponent инициализирован: масса=" << mass 
              << ", позиция=" << gameObject->transform->position.x << "," 
              << gameObject->transform->position.y << "," 
              << gameObject->transform->position.z << std::endl;
}

void RigidbodyComponent::syncWithPhysics() {
    if (!m_rigidbody || !gameObject) return;
    
    btTransform worldTransform;
    m_rigidbody->getMotionState()->getWorldTransform(worldTransform);
    
    // Обновляем Transform игрового объекта из физического тела
    btVector3 origin = worldTransform.getOrigin();
    gameObject->transform->position.x = origin.getX();
    gameObject->transform->position.y = origin.getY();
    gameObject->transform->position.z = origin.getZ();
    
    // Обновляем вращение (если нужно)
    btQuaternion rotation = worldTransform.getRotation();
    // Пока оставим вращение как есть, сосредоточимся на позиции
}

void RigidbodyComponent::beforePhysicsUpdate() {
    if (m_rigidbody) {
        btTransform trans;
        m_rigidbody->getMotionState()->getWorldTransform(trans);
        btVector3 pos = trans.getOrigin();
        m_prevPosition = glm::vec3(pos.getX(), pos.getY(), pos.getZ());
        btQuaternion rot = trans.getRotation();
        m_prevRotation = glm::quat(rot.getW(), rot.getX(), rot.getY(), rot.getZ());
    }
}

glm::mat4 RigidbodyComponent::getInterpolatedTransform(float alpha) {
    if (!m_rigidbody) return gameObject->transform->getModelMatrix();

    btTransform trans;
    m_rigidbody->getMotionState()->getWorldTransform(trans);
    btVector3 currentPosBt = trans.getOrigin();
    glm::vec3 currentPosition(currentPosBt.getX(), currentPosBt.getY(), currentPosBt.getZ());
    btQuaternion currentRotBt = trans.getRotation();
    glm::quat currentRotation(currentRotBt.getW(), currentRotBt.getX(), currentRotBt.getY(), currentRotBt.getZ());

    glm::vec3 interpolatedPosition = glm::mix(m_prevPosition, currentPosition, alpha);
    glm::quat interpolatedRotation = glm::slerp(m_prevRotation, currentRotation, alpha);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, interpolatedPosition);
    model *= glm::mat4_cast(interpolatedRotation);
    model = glm::scale(model, gameObject->transform->scale);
    return model;
}