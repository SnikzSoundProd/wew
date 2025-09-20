#pragma once
#include "Mesh.h"
#include "../animation/AnimData.h" // <-- ВМЕСТО ОБЪЯВЛЕНИЙ
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <map>
#include <string>

class Model {
public:
    Model() = default;
    Model(const std::string& path);
    void draw(std::shared_ptr<Shader> shader);
    auto& getBoneInfoMap() { return m_BoneInfoMap; }
    int& getBoneCount() { return m_BoneCounter; }
    std::vector<Mesh> meshes;
private:
    std::string directory;
    std::vector<TextureInfo> textures_loaded;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;
    void loadModel(const std::string& path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<TextureInfo> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
    void setVertexBoneData(Vertex& vertex, int boneID, float weight);
    void extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
};