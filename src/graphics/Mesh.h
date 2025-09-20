#pragma once
#include <vector>
#include <string>
#include <memory> // Для std::shared_ptr
#include <glm/glm.hpp>
#include "../graphics/Shader.h" // <-- Важно!
#include "../graphics/Texture2D.h"

#define MAX_BONE_INFLUENCE 4

// Новая структура для вершин
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;

    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];

    // --- НОВЫЙ КОНСТРУКТОР ДЛЯ ОЧИСТКИ ---
    Vertex() {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
            m_BoneIDs[i] = -1;
            m_Weights[i] = 0.0f;
        }
    }
};

// Новая структура для текстур, чтобы хранить тип
struct TextureInfo {
    unsigned int id;
    std::string type;
    std::string path; // Путь к файлу, полезно для отладки
};

class Mesh {
public:
    // Данные меша
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureInfo>  textures; // <-- Меш теперь хранит свои текстуры

    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<TextureInfo>& textures);
    void draw(std::shared_ptr<Shader> shader);

private:
    unsigned int VAO, VBO, EBO;
    void setupMesh();
};