/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2024 Media Design School

File Name : Mesh.cpp
Description : Implementations for Mesh class
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#include "Mesh.h"

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

		Shader.setInt(Name + Number, I);
		glBindTexture(GL_TEXTURE_2D, Textures[I].Id);
	}

	glBindVertexArray(MVao);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(Indices.size()), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void Mesh::cleanup() {
	// Delete VAO, VBO, and EBO
	if (MVao != 0) {
		glDeleteVertexArrays(1, &MVao);
		MVao = 0;
	}
	if (MVbo != 0) {
		glDeleteBuffers(1, &MVbo);
		MVbo = 0;
	}
	if (MEbo != 0) {
		glDeleteBuffers(1, &MEbo);
		MEbo = 0;
	}

	// Delete textures
	for (auto& texture : Textures) {
		if (texture.Id != 0) {
			glDeleteTextures(1, &texture.Id);
			texture.Id = 0;
		}
	}
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &MVao);
	glGenBuffers(1, &MVbo);
	glGenBuffers(1, &MEbo);

	glBindVertexArray(MVao);
	glBindBuffer(GL_ARRAY_BUFFER, MVbo);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), Vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), Indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
	                      reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));

	glBindVertexArray(0);
}
