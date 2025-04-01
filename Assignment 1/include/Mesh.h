/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2024 Media Design School

File Name : Mesh.h
Description : Definitions for model mesh in OpenGL
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#pragma once

#include "Shader.h"

#include <glew.h>
#include <glm.hpp>
#include <string>
#include <vector>

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;

	bool operator==(const Vertex& Other) const
	{
		return Position == Other.Position && Normal == Other.Normal && TexCoords == Other.TexCoords;
	}
};

namespace std
{
	template <>
	struct hash<glm::vec2>
	{
		size_t operator()(const glm::vec2& V) const noexcept
		{
			return hash<float>()(V.x) ^ (hash<float>()(V.y) << 1);
		}
	};

	template <>
	struct hash<glm::vec3>
	{
		size_t operator()(const glm::vec3& V) const noexcept
		{
			return hash<float>()(V.x) ^ (hash<float>()(V.y) << 1) ^ (hash<float>()(V.z) << 2);
		}
	};

	template <>
	struct hash<Vertex>
	{
		size_t operator()(const Vertex& Vertex) const noexcept
		{
			return ((hash<glm::vec3>()(Vertex.Position) ^
					(hash<glm::vec3>()(Vertex.Normal) << 1)) >> 1) ^
				(hash<glm::vec2>()(Vertex.TexCoords) << 1);
		}
	};
}

struct Texture
{
	unsigned int Id;
	std::string Type;
	std::string Path;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> Vertices, std::vector<unsigned int> Indices, std::vector<Texture> Textures);

	void draw(const Shader& Shader) const;
	void cleanup();  // Add this method to clean up the Mesh

	std::vector<Vertex> Vertices;
	std::vector<unsigned int> Indices;
	std::vector<Texture> Textures;

private:
	void setupMesh();

	unsigned int MVao;
	unsigned int MVbo;
	unsigned int MEbo;
};
