#pragma once
#include "../components/Component.h"
#include "../core/GameObject.h"
#include "../graphics/Model.h"
#include <memory>

class MeshRenderer : public Component {
public:
    std::shared_ptr<Model> model;
    std::shared_ptr<Shader> shader;

    std::string modelName;
    std::string modelPath;
    std::string shaderName;

    MeshRenderer() = default;

     MeshRenderer(std::shared_ptr<Model> m, std::string mName, std::string mPath, 
                 std::shared_ptr<Shader> s, std::string sName);

        void draw(const glm::mat4& view, const glm::mat4& projection); // Реализацию вынесем в .cpp

    void serialize(json& jsonData) override;
    void deserialize(const json& jsonData) override;
};