#pragma once
#include <btBulletDynamicsCommon.h>
#include <vector>
#include <glm/glm.hpp>

struct RaycastResult {
    bool hasHit = false;
    glm::vec3 point;
    btRigidBody* body = nullptr;
};

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
    RaycastResult raycast(const glm::vec3& from, const glm::vec3& to);

private:
    PhysicsEngine() = default;

    // Основные компоненты Bullet
    btDefaultCollisionConfiguration* m_collisionConfiguration = nullptr;
    btCollisionDispatcher* m_dispatcher = nullptr;
    btBroadphaseInterface* m_overlappingPairCache = nullptr;
    btSequentialImpulseConstraintSolver* m_solver = nullptr;
    btDiscreteDynamicsWorld* m_dynamicsWorld = nullptr;
};