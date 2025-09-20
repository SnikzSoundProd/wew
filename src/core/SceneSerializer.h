#pragma once
#include "Scene.h"
#include <string>

class SceneSerializer {
public:
    SceneSerializer(Scene& scene);

    void serialize(const std::string& filepath);
    bool deserialize(const std::string& filepath);

private:
    Scene& m_Scene;
};