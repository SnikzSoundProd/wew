#include "ResourceManager.h"
#include "../graphics/Model.h"

// Реализация синглтона (Meyers' Singleton) - потокобезопасно с C++11
ResourceManager& ResourceManager::GetInstance() {
    static ResourceManager instance;
    return instance;
}

std::shared_ptr<Model> ResourceManager::LoadModel(const std::string& name, const std::string& file) {
    if (m_Models.find(name) == m_Models.end()) {
        auto model = std::make_shared<Model>(file);
        m_Models[name] = model;
        return model;
    }
    return m_Models[name];
}

std::shared_ptr<Model> ResourceManager::GetModel(const std::string& name) {
    return m_Models[name];
}

std::shared_ptr<Shader> ResourceManager::LoadShader(const std::string& name, const char* vShaderFile, const char* fShaderFile) {
    // Проверяем, есть ли шейдер с таким именем на "складе"
    if (m_Shaders.find(name) == m_Shaders.end()) {
        // Если нет - загружаем, создаем и кладем на склад
        auto shader = std::make_shared<Shader>(vShaderFile, fShaderFile);
        m_Shaders[name] = shader;
        return shader;
    }
    // Если есть - просто возвращаем его
    return m_Shaders[name];
}

std::shared_ptr<Shader> ResourceManager::GetShader(const std::string& name) {
    return m_Shaders[name]; // В реальном проекте нужна проверка на существование
}

std::shared_ptr<Texture2D> ResourceManager::LoadTexture(const std::string& name, const std::string& file) {
    if (m_Textures.find(name) == m_Textures.end()) {
        auto texture = std::make_shared<Texture2D>();
        texture->Generate(file);
        m_Textures[name] = texture;
        return texture;
    }
    return m_Textures[name];
}

std::shared_ptr<Texture2D> ResourceManager::GetTexture(const std::string& name) {
    return m_Textures[name];
}

void ResourceManager::Clear() {
    // Очищаем все ресурсы
    m_Shaders.clear();
}