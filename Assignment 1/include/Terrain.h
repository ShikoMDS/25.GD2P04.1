#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <glew.h>
#include <glm.hpp>
#include "Mesh.h" // Include the Mesh.h file which already has the Vertex struct

// Structure to hold heightmap information
struct HeightMapInfo {
    std::string FilePath = "";
    unsigned int Width = 0;
    unsigned int Depth = 0;
    float CellSpacing = 1.0f;
};

// Terrain class definition
class Terrain {
public:
    Terrain(const HeightMapInfo& info);
    ~Terrain();

    // Setup and draw functions
    void SetupTerrain();  // Function to set up the terrain
    void DrawTerrain();   // Function to draw the terrain

private:
    HeightMapInfo terrainInfo;     // Terrain info
    std::vector<float> heightmap;  // Heightmap data
    GLuint vao, vbo, ebo;          // OpenGL buffers

    // Private functions for setting up and calculating the terrain
    void LoadHeightMap();  // Load heightmap from file
    void SmoothHeights();
    float Average(unsigned row, unsigned col);
    void SetupMesh();      // Setup VAO, VBO, and vertex data
    void SetupIndexBuffer(); // Setup EBO for indexed rendering
    void GenerateNormals(std::vector<Vertex>& Vertices); // Generate normals
};
