#pragma once
#include <string>

class Texture2D {
public:
    unsigned int ID;
    int Width, Height;

    Texture2D();
    void Generate(const std::string& file);
    void Bind() const;
};