#include "MeshRenderer.h"
#include "../core/GameObject.h"
#include "../core/ResourceManager.h" // Нужен для десериализации

void MeshRenderer::draw(const glm::mat4& view, const glm::mat4& projection) {
    if (!model || !shader || !gameObject) return;
    
    shader->use();
    shader->setMat4("model", gameObject->transform->getModelMatrix()); // <-- transform теперь указатель
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    
    model->draw(shader);
}

MeshRenderer::MeshRenderer(std::shared_ptr<Model> m, std::string mName, std::string mPath, 
                           std::shared_ptr<Shader> s, std::string sName)
    : model(m), modelName(mName), modelPath(mPath), shader(s), shaderName(sName) {}
    
void MeshRenderer::serialize(json& jsonData) {
    jsonData["modelName"] = modelName;
    jsonData["modelPath"] = modelPath;
    jsonData["shaderName"] = shaderName;
}

void MeshRenderer::deserialize(const json& jsonData) {
    modelName = jsonData["modelName"];
    modelPath = jsonData["modelPath"];
    shaderName = jsonData["shaderName"];
    
     // Собираем пути к шейдерам
    std::string vertPath = "shaders/" + shaderName + ".vert";
    std::string fragPath = "shaders/" + shaderName + ".frag";

     // Используем Load вместо Get!
    model = ResourceManager::GetInstance().LoadModel(modelName, modelPath);
    shader = ResourceManager::GetInstance().LoadShader(shaderName, vertPath.c_str(), fragPath.c_str());
}