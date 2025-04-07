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

    void draw(const Shader& shader, GLuint textureID) const;
    void cleanup();

private:
    void setupQuad();

    GLuint vao, vbo, ebo;
};