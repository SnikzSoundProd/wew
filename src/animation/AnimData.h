#pragma once

#include <glm/glm.hpp>

// Структура для хранения информации о вершине, на которую влияют кости
struct VertexWeight {
	unsigned int m_VertexID;
	float m_Weight;
};

// Структура BoneInfo была перенесена в Animation.h, так как она используется только там.

// ВОТ ОНО, РЕШЕНИЕ!
// Возвращаем BoneInfo сюда, чтобы избежать циклических зависимостей
struct BoneInfo
{
    int id;
    glm::mat4 offset;
};