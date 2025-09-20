#include "PhysicsEngine.h"

PhysicsEngine& PhysicsEngine::GetInstance() {
    static PhysicsEngine instance;
    return instance;
}

void PhysicsEngine::init() {
    // Настройка мира физики
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    m_overlappingPairCache = new btDbvtBroadphase();
    m_solver = new btSequentialImpulseConstraintSolver();
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration);

    // Устанавливаем гравитацию (направлена вниз по оси Y)
    m_dynamicsWorld->setGravity(btVector3(0, -9.81, 0));
}

void PhysicsEngine::update(float deltaTime) {
    if (m_dynamicsWorld) {
        // Делаем шаг симуляции
        m_dynamicsWorld->stepSimulation(deltaTime, 10);
    }
}

void PhysicsEngine::shutdown() {
    // Важно удалять все в обратном порядке создания!
    delete m_dynamicsWorld;
    delete m_solver;
    delete m_overlappingPairCache;
    delete m_dispatcher;
    delete m_collisionConfiguration;
}

void PhysicsEngine::addRigidbody(btRigidBody* body) {
    if (m_dynamicsWorld) {
        m_dynamicsWorld->addRigidBody(body);
    }
}

RaycastResult PhysicsEngine::raycast(const glm::vec3& from, const glm::vec3& to) {
    btVector3 btFrom(from.x, from.y, from.z);
    btVector3 btTo(to.x, to.y, to.z);

    btCollisionWorld::ClosestRayResultCallback closestResults(btFrom, btTo);
    
    // Запускаем тест
    m_dynamicsWorld->rayTest(btFrom, btTo, closestResults);

    RaycastResult result;
    if (closestResults.hasHit()) {
        result.hasHit = true;
        result.point = glm::vec3(closestResults.m_hitPointWorld.x(), closestResults.m_hitPointWorld.y(), closestResults.m_hitPointWorld.z());
        result.body = (btRigidBody*)btRigidBody::upcast(closestResults.m_collisionObject);
    }

    return result;
}