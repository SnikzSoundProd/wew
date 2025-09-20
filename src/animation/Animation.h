// --- FULL FIX FOR: Animation.h ---
#pragma once
#include <map>
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "Bone.h" 
#include "AnimData.h"

// Прямые объявления
class Model;
struct aiAnimation;
struct aiNode;

struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class Animation
{
public:
	Animation() = default;
	Animation(const std::string& animationPath, Model* model);
	~Animation() = default;

	Bone* findBone(const std::string& name);

	// --- ИСПРАВЛЕННЫЕ ГЕТТЕРЫ ---
	inline float getTicksPerSecond() const { return m_TicksPerSecond; }
	inline float getDuration() const { return m_Duration; }
	inline const AssimpNodeData& getRootNode() const { return m_RootNode; }
	inline const std::map<std::string, BoneInfo>& getBoneIDMap() const
	{
		return m_BoneInfoMap;
	}

private:
	// --- ИСПРАВЛЕННЫЕ СИГНАТУРЫ ---
	void readMissingBones(const aiAnimation* animation, Model& model);
	void readHierarchyData(AssimpNodeData& dest, const aiNode* src);

	float m_Duration = 0.0f;
	float m_TicksPerSecond = 0.0f;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	std::map<std::string, BoneInfo> m_BoneInfoMap;
};