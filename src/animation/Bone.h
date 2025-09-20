// --- FULL FIX FOR: Bone.h ---
#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp> // <-- Добавлен для translate/scale
#include "KeyFrame.h"

// Прямое объявление, чтобы компилятор знал о нем
struct aiNodeAnim;

struct Bone
{
	// --- ПУБЛИЧНЫЕ ДАННЫЕ (как в твоей версии) ---
	std::vector<KeyPosition> m_Positions;
	std::vector<KeyRotation> m_Rotations;
	std::vector<KeyScale> m_Scales;
	glm::mat4 m_LocalTransform;
	std::string m_Name;
	int m_ID;

	// --- КОНСТРУКТОРЫ ---
	Bone(); // Конструктор по умолчанию
	// Конструктор, который будет вызываться при загрузке модели
	Bone(const std::string& name, int ID, const aiNodeAnim* channel); 

	// --- ОСНОВНЫЕ МЕТОДЫ ---
	void Update(float animationTime); // <--- Переименовал в Update с большой буквы, как у тебя

	// --- ГЕТТЕРЫ, ЧТОБЫ Animation.cpp мог их использовать ---
	glm::mat4 getLocalTransform() const { return m_LocalTransform; }
	const std::string& getBoneName() const { return m_Name; }
	int getBoneID() const { return m_ID; }

	// --- ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ПОИСКА ИНДЕКСА (как у тебя) ---
	int GetPositionIndex(float animationTime) const;
	int GetRotationIndex(float animationTime) const;
	int GetScaleIndex(float animationTime) const;

private:
	// --- ПРИВАТНЫЕ МЕТОДЫ ИНТЕРПОЛЯЦИИ (как у тебя) ---
	float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const;
	glm::mat4 InterpolatePosition(float animationTime);
	glm::mat4 InterpolateRotation(float animationTime);
	glm::mat4 InterpolateScaling(float animationTime);
};