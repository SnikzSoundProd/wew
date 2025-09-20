#pragma once

#include <map>
#include <string>
#include <memory>
#include "../graphics/Texture2D.h"
#include "../graphics/Shader.h"
#include "../graphics/Model.h"

class ResourceManager {
public:
    // Запрещаем копирование и присваивание
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // Статический метод для доступа к единственному экземпляру
    static ResourceManager& GetInstance();

    std::shared_ptr<Model> LoadModel(const std::string& name, const std::string& file);
    std::shared_ptr<Model> GetModel(const std::string& name);
    std::shared_ptr<Texture2D> LoadTexture(const std::string& name, const std::string& file);
    std::shared_ptr<Texture2D> GetTexture(const std::string& name);

    // Загружает (или находит уже загруженный) шейдер
    std::shared_ptr<Shader> LoadShader(const std::string& name, const char* vShaderFile, const char* fShaderFile);
    
    // Возвращает уже загруженный шейдер
    std::shared_ptr<Shader> GetShader(const std::string& name);
    
    // Выгружает все ресурсы
    void Clear();

private:
    // Приватный конструктор, чтобы никто не мог создать экземпляр напрямую
    ResourceManager() = default;

    // "Склад" для шейдеров
    std::map<std::string, std::shared_ptr<Shader>> m_Shaders;
    std::map<std::string, std::shared_ptr<Texture2D>> m_Textures;
    std::map<std::string, std::shared_ptr<Model>> m_Models;
};