#pragma once
#include <btBulletDynamicsCommon.h>
#include <vector>

class PhysicsEngine {
public:
    PhysicsEngine(const PhysicsEngine&) = delete;
    PhysicsEngine& operator=(const PhysicsEngine&) = delete;

    static PhysicsEngine& GetInstance();

    void init();
    void update(float deltaTime);
    void shutdown();

    void addRigidbody(btRigidBody* body);
    btDiscreteDynamicsWorld* getDynamicsWorld() const { return m_dynamicsWorld; }

private:
    PhysicsEngine() = default;

    // Основные компоненты Bullet
    btDefaultCollisionConfiguration* m_collisionConfiguration = nullptr;
    btCollisionDispatcher* m_dispatcher = nullptr;
    btBroadphaseInterface* m_overlappingPairCache = nullptr;
    btSequentialImpulseConstraintSolver* m_solver = nullptr;
    btDiscreteDynamicsWorld* m_dynamicsWorld = nullptr;
};