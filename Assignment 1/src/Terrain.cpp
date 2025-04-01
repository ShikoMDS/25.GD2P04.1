#include "Terrain.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <glm.hpp>
#include <glew.h>

// Constructor for Terrain, takes in HeightMapInfo
Terrain::Terrain(const HeightMapInfo& info) : terrainInfo(info) {
    LoadHeightMap();  // Load the heightmap data
    SmoothHeights();  // Apply smoothing
    SmoothHeights();  // Apply multiple times
    SmoothHeights();
    SmoothHeights();
    SmoothHeights();
    SetupTerrain();   // Set up the terrain mesh
}

// Destructor for Terrain, cleans up buffers
Terrain::~Terrain() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

// Function to load heightmap from a raw file
void Terrain::LoadHeightMap() {
    terrainInfo.Width = 512;  // Set the terrain width to match heightmap resolution
    terrainInfo.Depth = 512;  // Set the terrain depth to match heightmap resolution

    unsigned int VertexCount = terrainInfo.Width * terrainInfo.Depth;

    // Temporary vector to hold raw byte data
    std::vector<unsigned char> HeightValue(VertexCount);

    // Open the file in binary mode
    std::ifstream file(terrainInfo.FilePath, std::ios_base::binary);
    if (file) {
        file.read(reinterpret_cast<char*>(&HeightValue[0]), static_cast<std::streamsize>(HeightValue.size()));
        file.close();
    }
    else {
        std::cerr << "Error: Could not load heightmap file: " << terrainInfo.FilePath << std::endl;
        return;
    }

    // Normalize heightmap values to [0, 1]
    heightmap.resize(VertexCount, 0.0f);
    for (unsigned int i = 0; i < VertexCount; i++) {
        heightmap[i] = static_cast<float>(HeightValue[i]) / 255.0f;  // Normalize byte values to [0, 1]
    }
}

// Function to smooth heightmap by averaging neighboring heights
void Terrain::SmoothHeights() {
    std::vector<float> smoothedMap(heightmap.size());

    for (unsigned int row = 0; row < terrainInfo.Width; row++) {
        for (unsigned int col = 0; col < terrainInfo.Depth; col++) {
            smoothedMap[row * terrainInfo.Depth + col] = Average(row, col);
        }
    }

    heightmap = smoothedMap;
}

// Helper function to calculate the average height of neighboring vertices
float Terrain::Average(unsigned int row, unsigned int col) {
    float sum = 0.0f;
    int count = 0;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int newRow = row + i;
            int newCol = col + j;

            if (newRow >= 0 && newRow < (int)terrainInfo.Width && newCol >= 0 && newCol < (int)terrainInfo.Depth) {
                sum += heightmap[newRow * terrainInfo.Depth + newCol];
                count++;
            }
        }
    }
    return (sum / count);
}

// Function to set up the terrain mesh (vertices, indices, normals)
void Terrain::SetupTerrain() {
    SetupMesh();  // Set up the vertex positions, normals, and texture coordinates
}

// Function to generate vertex positions, texture coordinates, and normals
void Terrain::SetupMesh() {
    unsigned int VertexCount = terrainInfo.Width * terrainInfo.Depth;
    std::vector<Vertex> Vertices(VertexCount);

    float HalfWidth = (terrainInfo.Width - 1) * terrainInfo.CellSpacing * 0.5f;
    float HalfDepth = (terrainInfo.Depth - 1) * terrainInfo.CellSpacing * 0.5f;
    float HeightScale = 1000.0f;  // Adjust as needed

    // Iterate through the terrain grid and assign height values from the heightmap
    for (unsigned int row = 0; row < terrainInfo.Depth; row++) {
        float PosZ = HalfDepth - (row * terrainInfo.CellSpacing);  // Z position (depth)

        for (unsigned int col = 0; col < terrainInfo.Width; col++) {
            unsigned int Index = row * terrainInfo.Width + col;  // Index in heightmap

            float PosX = -HalfWidth + (col * terrainInfo.CellSpacing);  // X position (width)
            float PosY = heightmap[Index] * HeightScale;  // Apply heightmap to Y

            // Set the vertex position (X, Y, Z)
            Vertices[Index].Position = glm::vec3(PosX, PosY, PosZ);
        }
    }


    // Generate normals for the terrain vertices
    GenerateNormals(Vertices);

    // Set up OpenGL buffers (VAO, VBO, EBO)
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);

    // Fill VBO with vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);

    // Set up vertex attributes (position, texture coordinates, normals)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords)); // TexCoord
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal)); // Normal
    glEnableVertexAttribArray(2);

    SetupIndexBuffer();

    glBindVertexArray(0);
}

// Function to generate normals for the terrain vertices
void Terrain::GenerateNormals(std::vector<Vertex>& Vertices) {
    float inverseCellSpacing = 1.0f / (2.0f * terrainInfo.CellSpacing);

    for (unsigned int row = 0; row < terrainInfo.Width; row++) {
        for (unsigned int col = 0; col < terrainInfo.Depth; col++) {
            float rowNeg = (row == 0) ? heightmap[row * terrainInfo.Depth + col] : heightmap[(row - 1) * terrainInfo.Depth + col];
            float rowPos = (row == terrainInfo.Width - 1) ? heightmap[row * terrainInfo.Depth + col] : heightmap[(row + 1) * terrainInfo.Depth + col];
            float colNeg = (col == 0) ? heightmap[row * terrainInfo.Depth + col] : heightmap[row * terrainInfo.Depth + (col - 1)];
            float colPos = (col == terrainInfo.Depth - 1) ? heightmap[row * terrainInfo.Depth + col] : heightmap[row * terrainInfo.Depth + (col + 1)];

            float x = rowNeg - rowPos;
            if (row == 0 || row == terrainInfo.Width - 1) x *= 2.0f;

            float y = colPos - colNeg;
            if (col == 0 || col == terrainInfo.Depth - 1) y *= 2.0f;

            glm::vec3 tangentZ(0.0f, x * inverseCellSpacing, 1.0f);
            glm::vec3 tangentX(1.0f, y * inverseCellSpacing, 0.0f);

            glm::vec3 normal = glm::cross(tangentZ, tangentX);
            normal = glm::normalize(normal);

            Vertices[row * terrainInfo.Depth + col].Normal = normal;
        }
    }
}

// Function to set up the index buffer (EBO)
void Terrain::SetupIndexBuffer() {
    unsigned int FaceCount = (terrainInfo.Width - 1) * (terrainInfo.Depth - 1) * 2;
    unsigned int DrawCount = FaceCount * 3; // 3 indices per triangle
    std::vector<GLuint> Indices(DrawCount);

    int Index = 0;
    for (unsigned int row = 0; row < (terrainInfo.Depth - 1); row++) {
        for (unsigned int col = 0; col < (terrainInfo.Width - 1); col++) {
            Indices[Index++] = row * terrainInfo.Width + col;
            Indices[Index++] = (row + 1) * terrainInfo.Width + col;
            Indices[Index++] = row * terrainInfo.Width + (col + 1);

            Indices[Index++] = row * terrainInfo.Width + (col + 1);
            Indices[Index++] = (row + 1) * terrainInfo.Width + col;
            Indices[Index++] = (row + 1) * terrainInfo.Width + (col + 1);
        }
    }

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(GLuint), &Indices[0], GL_STATIC_DRAW);
}

// Function to render the terrain
void Terrain::DrawTerrain() {
    glBindVertexArray(vao);
    glCullFace(GL_FRONT);
    glDrawElements(GL_TRIANGLES, (terrainInfo.Width - 1) * (terrainInfo.Depth - 1) * 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
