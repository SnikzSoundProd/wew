#pragma once
#include "ColliderComponent.h"
#include <glm/glm.hpp>

class BoxColliderComponent : public ColliderComponent {
public:
    glm::vec3 size = {1.0f, 1.0f, 1.0f};

    btCollisionShape* createShape() override {
        return new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));
    }
};