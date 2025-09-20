#pragma once
#include "../components/Component.h"
#include <btBulletCollisionCommon.h>

class ColliderComponent : public Component {
public:
    virtual ~ColliderComponent() = default;
    // Каждый коллайдер должен уметь создавать свою btCollisionShape
    virtual btCollisionShape* createShape() = 0;
};