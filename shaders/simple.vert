#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

// Выходные переменные для фрагментного шейдера
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords; // <-- НОВЫЙ ВЫХОД

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0)); // <-- Позиция фрагмента в мировых координатах
    // Нормаль тоже нужно повернуть вместе с объектом, но без сдвигов и масштабирования
    Normal = mat3(transpose(inverse(model))) * aNormal; 
    TexCoords = aTexCoords;
}