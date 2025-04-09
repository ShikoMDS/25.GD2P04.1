/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2024 Media Design School

File Name : Quad.cpp
Description : Implementation of Quad class for texture rendering
**************************************************************************/

#include "Quad.h"
#include <iostream>

Quad::Quad() : PvVao(0), PvVbo(0), PvEbo(0) {
    setupQuad();
}

Quad::~Quad() {
    cleanup();
}

void Quad::setupQuad() {
    // Vertex data for a quad (position, normal, tex coords)
    constexpr float Vertices[] = {
        // positions          // normals           // texture coords
        -1.0f,  1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f, // top left
         1.0f,  1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f  // bottom left
    };

    // Indices for drawing the quad as two triangles
    const unsigned int Indices[] = {
        0, 1, 2, // first triangle
        0, 2, 3  // second triangle
    };

    glGenVertexArrays(1, &PvVao);
    glGenBuffers(1, &PvVbo);
    glGenBuffers(1, &PvEbo);

    glBindVertexArray(PvVao);

    // Load vertex data
    glBindBuffer(GL_ARRAY_BUFFER, PvVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    // Load index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, PvEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));

    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    // Texture coords attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);
}

void Quad::draw(const Shader& Shader, GLuint TextureId) const {
    // DON'T activate the shader here - it should already be active
    // Draw quad
    glBindVertexArray(PvVao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Quad::cleanup() {
    if (PvVao != 0) {
        glDeleteVertexArrays(1, &PvVao);
        PvVao = 0;
    }
    if (PvVbo != 0) {
        glDeleteBuffers(1, &PvVbo);
        PvVbo = 0;
    }
    if (PvEbo != 0) {
        glDeleteBuffers(1, &PvEbo);
        PvEbo = 0;
    }
}