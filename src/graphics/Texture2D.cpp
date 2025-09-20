#include "Texture2D.h"
#include <glad/glad.h>
#include <iostream>
#include "../vendor/stb_image.h" // Подключаем наш загрузчик

Texture2D::Texture2D() : Width(0), Height(0), ID(0) {}

void Texture2D::Generate(const std::string& file) {
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    // Устанавливаем параметры наложения и фильтрации
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int nrChannels;
    // ВАЖНО: OpenGL ожидает, что 0-ая Y-координата находится внизу, а изображения обычно хранят ее наверху.
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(file.c_str(), &Width, &Height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, Width, Height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture: " << file << std::endl;
    }
    stbi_image_free(data);
}

void Texture2D::Bind() const {
    glBindTexture(GL_TEXTURE_2D, ID);
}