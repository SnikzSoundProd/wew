#pragma once
#include "../components/Component.h"
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp> // <-- ГЛАВНЫЙ ФИКС №1
#include <glm/gtc/quaternion.hpp> // <-- ГЛАВНЫЙ ФИКС №2

class RigidbodyComponent : public Component {
public:
    float mass = 1.0f;

    RigidbodyComponent();
    ~RigidbodyComponent();

    void init(); // Инициализация после добавления к GameObject
    void syncWithPhysics();

    btRigidBody* getBtRigidbody() const { return m_rigidbody; }

    void beforePhysicsUpdate(); // Запомнить состояние до шага физики
glm::mat4 getInterpolatedTransform(float alpha); // Получить сглаженную матрицу

private:
    btRigidBody* m_rigidbody = nullptr;
    btDefaultMotionState* m_motionState = nullptr;

    glm::vec3 m_prevPosition;
    glm::quat m_prevRotation;
};