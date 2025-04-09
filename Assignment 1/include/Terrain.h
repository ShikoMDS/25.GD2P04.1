/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2024 Media Design School

File Name : Terrain.h
Description : Declarations for terrain rendering in OpenGL program
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#pragma once

#include "Mesh.h"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <glew.h>
#include <glm.hpp>

struct HeightMapInfo
{
	std::string FilePath;
	unsigned int Width = 0;
	unsigned int Depth = 0;
	float CellSpacing = 1.0f;
};

class Terrain
{
public:
	explicit Terrain(const HeightMapInfo& Info);
	~Terrain();

	Terrain(const Terrain& Other) = delete;
	Terrain& operator=(const Terrain& Other) = delete;
	Terrain(Terrain&& Other) noexcept = delete;
	Terrain& operator=(Terrain&& Other) noexcept = delete;

	void setupTerrain();
	void drawTerrain() const;

private:
	HeightMapInfo PvTerrainInfo;
	std::vector<float> PvHeightmap;
	GLuint PvVao, PvVbo, PvEbo;

	void loadHeightMap();
	void smoothHeights();
	[[nodiscard]] float average(unsigned Row, unsigned Col) const;
	void setupMesh();
	void setupIndexBuffer();
	void generateNormals(std::vector<Vertex>& Vertices) const;
};
