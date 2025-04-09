/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2025 Media Design School

File Name : Mesh.cpp
Description : Implementations for Mesh class
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#include "Mesh.h"

#include <iostream>

Mesh::Mesh(std::vector<Vertex> Vertices, std::vector<unsigned int> Indices, std::vector<Texture> Textures)
	: Vertices(std::move(Vertices)), Indices(std::move(Indices)), Textures(std::move(Textures))
{
	setupMesh();
}

void Mesh::draw(const Shader& Shader) const
{
	unsigned int DiffuseNr = 1;
	unsigned int SpecularNr = 1;
	for (unsigned int I = 0; I < Textures.size(); I++)
	{
		glActiveTexture(GL_TEXTURE0 + I);
		std::string Number;
		std::string Name = Textures[I].Type;
		if (Name == "texture_diffuse")
			Number = std::to_string(DiffuseNr++);
		else if (Name == "texture_specular")
			Number = std::to_string(SpecularNr++);

		Shader.setInt(Name + Number, static_cast<int>(I));
		glBindTexture(GL_TEXTURE_2D, Textures[I].Id);

		if (Textures[I].Id == 0)
		{
			std::cout << "Warning: texture " << Textures[I].Path << " failed to load." << '\n';
		}
	}

	glBindVertexArray(PvVao);
	glDrawElements(GL_TRIANGLES, static_cast<int>(Indices.size()), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void Mesh::cleanup()
{
	if (PvVao != 0)
	{
		glDeleteVertexArrays(1, &PvVao);
		PvVao = 0;
	}
	if (PvVbo != 0)
	{
		glDeleteBuffers(1, &PvVbo);
		PvVbo = 0;
	}
	if (PvEbo != 0)
	{
		glDeleteBuffers(1, &PvEbo);
		PvEbo = 0;
	}

	for (auto& Texture : Textures)
	{
		if (Texture.Id != 0)
		{
			glDeleteTextures(1, &Texture.Id);
			Texture.Id = 0;
		}
	}
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &PvVao);
	glGenBuffers(1, &PvVbo);
	glGenBuffers(1, &PvEbo);

	glBindVertexArray(PvVao);
	glBindBuffer(GL_ARRAY_BUFFER, PvVbo);
	glBufferData(GL_ARRAY_BUFFER, static_cast<long long>(Vertices.size() * sizeof(Vertex)), Vertices.data(),
	             GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, PvEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long long>(Indices.size() * sizeof(unsigned int)), Indices.data(),
	             GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
	                      reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));

	glBindVertexArray(0);
}
