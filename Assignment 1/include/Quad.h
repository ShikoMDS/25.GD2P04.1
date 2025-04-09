/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2024 Media Design School

File Name : Quad.h
Description : Quad rendering for displaying textures and effects
**************************************************************************/

#pragma once

#include <glew.h>
#include <vector>
#include "Shader.h"

class Quad {
public:
    Quad();
    ~Quad();

    Quad(const Quad& Other) = delete;
    Quad& operator=(const Quad& Other) = delete;
    Quad(Quad&& Other) noexcept = delete;
    Quad& operator=(Quad&& Other) noexcept = delete;

    void draw(const Shader& Shader, GLuint TextureId) const;
    void cleanup();

private:
    void setupQuad();

    GLuint PvVao, PvVbo, PvEbo;
};