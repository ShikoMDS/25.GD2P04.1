#include "Terrain.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <glm.hpp>
#include <glew.h>

// Constructor for Terrain, takes in HeightMapInfo
Terrain::Terrain(const HeightMapInfo& info) : terrainInfo(info) {
    std::cout << "Initializing terrain from heightmap: " << info.FilePath << std::endl;
    LoadHeightMap();  // Load the heightmap data
    
    // Apply smoothing multiple times for better results
    std::cout << "Smoothing heightmap data..." << std::endl;
    for (int i = 0; i < 5; i++) {
        SmoothHeights();
    }
    
    SetupTerrain();   // Set up the terrain mesh
    std::cout << "Terrain initialization complete" << std::endl;
}

// Destructor for Terrain, cleans up buffers
Terrain::~Terrain() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

// Function to load heightmap from a raw file
void Terrain::LoadHeightMap() {
    unsigned int VertexCount = terrainInfo.Width * terrainInfo.Depth;
    std::cout << "Loading heightmap with dimensions: " << terrainInfo.Width << "x" << terrainInfo.Depth << std::endl;

    // Temporary vector to hold raw byte data
    std::vector<unsigned char> HeightValue(VertexCount);

    // Open the file in binary mode
    std::ifstream file(terrainInfo.FilePath, std::ios_base::binary);
    if (file) {
        file.read(reinterpret_cast<char*>(&HeightValue[0]), static_cast<std::streamsize>(HeightValue.size()));
        file.close();
        std::cout << "Heightmap file loaded successfully" << std::endl;
    }
    else {
        std::cerr << "Error: Could not load heightmap file: " << terrainInfo.FilePath << std::endl;
        // Initialize with default heightmap if file loading fails
        for (unsigned int i = 0; i < VertexCount; i++) {
            HeightValue[i] = 0;
        }
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
    float totalWeight = 0.0f;

    // Use a larger kernel for better smoothing (5x5)
    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            int newRow = row + i;
            int newCol = col + j;

            if (newRow >= 0 && newRow < (int)terrainInfo.Width && newCol >= 0 && newCol < (int)terrainInfo.Depth) {
                // Weight samples based on distance for more natural smoothing
                float distance = sqrt(float(i*i + j*j));
                float weight = 1.0f / (1.0f + distance);
                sum += heightmap[newRow * terrainInfo.Depth + newCol] * weight;
                totalWeight += weight;
            }
        }
    }
    return (totalWeight > 0) ? (sum / totalWeight) : 0.0f;
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
    float HeightScale = 2000.0f;  // Significantly increased height for more dramatic mountains

    std::cout << "Setting up terrain mesh..." << std::endl;

    // Iterate through the terrain grid and assign height values from the heightmap
    for (unsigned int row = 0; row < terrainInfo.Depth; row++) {
        float PosZ = HalfDepth - (row * terrainInfo.CellSpacing);  // Z position (depth)

        for (unsigned int col = 0; col < terrainInfo.Width; col++) {
            unsigned int Index = row * terrainInfo.Width + col;  // Index in heightmap

            float PosX = -HalfWidth + (col * terrainInfo.CellSpacing);  // X position (width)
            float PosY = heightmap[Index] * HeightScale;  // Apply heightmap to Y

            // Set the vertex position (X, Y, Z)
            Vertices[Index].Position = glm::vec3(PosX, PosY, PosZ);
            
            // Set texture coordinates (normalized)
            Vertices[Index].TexCoords = glm::vec2(
                static_cast<float>(col) / static_cast<float>(terrainInfo.Width - 1),
                static_cast<float>(row) / static_cast<float>(terrainInfo.Depth - 1)
            );
        }
    }

    // Generate normals for the terrain vertices
    GenerateNormals(Vertices);
    std::cout << "Terrain normals generated" << std::endl;

    // Set up OpenGL buffers (VAO, VBO, EBO)
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);

    // Fill VBO with vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);

    // Set up vertex attributes (position, normal, texcoords)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal)); // Normal
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords)); // TexCoord
    glEnableVertexAttribArray(2);

    SetupIndexBuffer();
    std::cout << "Terrain mesh setup complete" << std::endl;

    glBindVertexArray(0);
}

// Function to generate normals for the terrain vertices
void Terrain::GenerateNormals(std::vector<Vertex>& Vertices) {
    // Initialize normals to zero
    for (auto& vertex : Vertices) {
        vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    // For each vertex, calculate normal by averaging the normals of adjacent faces
    for (unsigned int z = 0; z < terrainInfo.Depth - 1; ++z) {
        for (unsigned int x = 0; x < terrainInfo.Width - 1; ++x) {
            // Indices of the four corners of the current quad
            unsigned int topLeft = z * terrainInfo.Width + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (z + 1) * terrainInfo.Width + x;
            unsigned int bottomRight = bottomLeft + 1;

            // Vertices of the four corners
            glm::vec3 vTL = Vertices[topLeft].Position;
            glm::vec3 vTR = Vertices[topRight].Position;
            glm::vec3 vBL = Vertices[bottomLeft].Position;
            glm::vec3 vBR = Vertices[bottomRight].Position;

            // Calculate normal for first triangle (TL, BL, TR)
            glm::vec3 v1 = vBL - vTL;
            glm::vec3 v2 = vTR - vTL;
            glm::vec3 normal1 = glm::normalize(glm::cross(v1, v2));

            // Calculate normal for second triangle (TR, BL, BR)
            glm::vec3 v3 = vBR - vTR;
            glm::vec3 v4 = vBL - vTR;
            glm::vec3 normal2 = glm::normalize(glm::cross(v3, v4));

            // Add these normals to each of the involved vertices
            // Later we'll normalize them after all contributions are summed
            Vertices[topLeft].Normal += normal1;
            Vertices[topRight].Normal += normal1;
            Vertices[topRight].Normal += normal2;
            Vertices[bottomLeft].Normal += normal1;
            Vertices[bottomLeft].Normal += normal2;
            Vertices[bottomRight].Normal += normal2;
        }
    }

    // Normalize all normals
    for (auto& vertex : Vertices) {
        if (glm::length(vertex.Normal) > 0.0f) {
            vertex.Normal = glm::normalize(vertex.Normal);
        } else {
            // Default normal pointing upward if no face contributions
            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
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
// Function to render the terrain
void Terrain::DrawTerrain() {
    // First save current OpenGL state
    GLboolean cullFaceEnabled;
    GLint cullFaceMode;
    glGetBooleanv(GL_CULL_FACE, &cullFaceEnabled);
    glGetIntegerv(GL_CULL_FACE_MODE, &cullFaceMode);

    // Disable culling completely for terrain rendering
    glDisable(GL_CULL_FACE);

    // Bind the vertex array object and draw
    glBindVertexArray(vao);

    // Draw the terrain using indices
    glDrawElements(GL_TRIANGLES, (terrainInfo.Width - 1) * (terrainInfo.Depth - 1) * 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    // Restore previous OpenGL state
    if (cullFaceEnabled) {
        glEnable(GL_CULL_FACE);
        glCullFace(cullFaceMode);
    }
}