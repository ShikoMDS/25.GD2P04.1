/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2024 Media Design School

File Name : Terrain.cpp
Description : Implementations for Terrain class
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#include "Terrain.h"

Terrain::Terrain(const HeightMapInfo& Info) : PvTerrainInfo(Info)
{
	std::cout << "Initializing terrain from heightmap: " << Info.FilePath << '\n';
	loadHeightMap();

	// Apply smoothing multiple times for better results
	//std::cout << "Smoothing heightmap data..." << '\n';
	for (int I = 0; I < 5; I++)
	{
		smoothHeights();
	}

	setupTerrain();
	//std::cout << "Terrain initialization complete" << '\n';
}

Terrain::~Terrain()
{
	glDeleteVertexArrays(1, &PvVao);
	glDeleteBuffers(1, &PvVbo);
	glDeleteBuffers(1, &PvEbo);
}

void Terrain::loadHeightMap()
{
	const unsigned int VertexCount = PvTerrainInfo.Width * PvTerrainInfo.Depth;
	//std::cout << "Loading heightmap with dimensions: " << PvTerrainInfo.Width << "x" << PvTerrainInfo.Depth << '\n';

	// Temporary vector to hold raw byte data
	std::vector<unsigned char> HeightValue(VertexCount);

	// Open file in binary mode
	if (std::ifstream File(PvTerrainInfo.FilePath, std::ios_base::binary); File)
	{
		File.read(reinterpret_cast<char*>(HeightValue.data()), static_cast<std::streamsize>(HeightValue.size()));
		File.close();
		//std::cout << "Heightmap file loaded successfully" << '\n';
	}
	else
	{
		std::cerr << "Error: Could not load heightmap file: " << PvTerrainInfo.FilePath << '\n';
		// Initialize with default heightmap if file loading fails
		for (unsigned int I = 0; I < VertexCount; I++)
		{
			HeightValue[I] = 0;
		}
		return;
	}

	// Normalize heightmap values to [0, 1]
	PvHeightmap.resize(VertexCount, 0.0f);
	for (unsigned int I = 0; I < VertexCount; I++)
	{
		PvHeightmap[I] = static_cast<float>(HeightValue[I]) / 255.0f; // Normalize byte values to [0, 1]
	}
}

void Terrain::smoothHeights()
{
	std::vector<float> SmoothedMap(PvHeightmap.size());

	for (unsigned int Row = 0; Row < PvTerrainInfo.Width; Row++)
	{
		for (unsigned int Col = 0; Col < PvTerrainInfo.Depth; Col++)
		{
			SmoothedMap[Row * PvTerrainInfo.Depth + Col] = average(Row, Col);
		}
	}

	PvHeightmap = SmoothedMap;
}

float Terrain::average(const unsigned int Row, const unsigned int Col) const
{
	float Sum = 0.0f;
	float TotalWeight = 0.0f;

	// Use larger kernel for better smoothing (5x5)
	for (int I = -2; I <= 2; I++)
	{
		for (int J = -2; J <= 2; J++)
		{
			const int NewRow = static_cast<int>(Row) + I;
			const int NewCol = static_cast<int>(Col) + J;

			if (NewRow >= 0 && NewRow < static_cast<int>(PvTerrainInfo.Width) && NewCol >= 0 && NewCol < static_cast<
				int>(PvTerrainInfo.Depth))
			{
				// Weight samples based on distance for more natural smoothing
				const float Distance = sqrt(static_cast<float>(I * I + J * J));
				const float Weight = 1.0f / (1.0f + Distance);
				Sum += PvHeightmap[NewRow * PvTerrainInfo.Depth + NewCol] * Weight;
				TotalWeight += Weight;
			}
		}
	}
	return (TotalWeight > 0) ? (Sum / TotalWeight) : 0.0f;
}

void Terrain::setupTerrain()
{
	setupMesh();
}

void Terrain::setupMesh()
{
	const unsigned int VertexCount = PvTerrainInfo.Width * PvTerrainInfo.Depth;
	std::vector<Vertex> Vertices(VertexCount);

	const float HalfWidth = static_cast<float>(PvTerrainInfo.Width - 1) * PvTerrainInfo.CellSpacing * 0.5f;
	const float HalfDepth = static_cast<float>(PvTerrainInfo.Depth - 1) * PvTerrainInfo.CellSpacing * 0.5f;
	constexpr float HeightScale = 2000.0f;

	//std::cout << "Setting up terrain mesh..." << '\n';

	// Iterate through terrain grid and assign height values from heightmap
	for (unsigned int Row = 0; Row < PvTerrainInfo.Depth; Row++)
	{
		const float PosZ = HalfDepth - static_cast<float>(Row) * PvTerrainInfo.CellSpacing; // Z position (depth)

		for (unsigned int Col = 0; Col < PvTerrainInfo.Width; Col++)
		{
			const unsigned int Index = Row * PvTerrainInfo.Width + Col; // Index in heightmap

			const float PosX = -HalfWidth + static_cast<float>(Col) * PvTerrainInfo.CellSpacing; // X position (width)
			const float PosY = PvHeightmap[Index] * HeightScale; // Apply heightmap to Y

			// Set vertex position (X, Y, Z)
			Vertices[Index].Position = glm::vec3(PosX, PosY, PosZ);

			// Set texture coordinates (normalized)
			Vertices[Index].TexCoords = glm::vec2(
				static_cast<float>(Col) / static_cast<float>(PvTerrainInfo.Width - 1),
				static_cast<float>(Row) / static_cast<float>(PvTerrainInfo.Depth - 1)
			);
		}
	}

	generateNormals(Vertices);
	//std::cout << "Terrain normals generated" << '\n';

	glGenVertexArrays(1, &PvVao);
	glGenBuffers(1, &PvVbo);
	glBindVertexArray(PvVao);

	glBindBuffer(GL_ARRAY_BUFFER, PvVbo);
	glBufferData(GL_ARRAY_BUFFER, static_cast<long long>(Vertices.size() * sizeof(Vertex)), Vertices.data(),
	             GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
	                      reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));
	glEnableVertexAttribArray(2);

	setupIndexBuffer();
	//std::cout << "Terrain mesh setup complete" << '\n';

	glBindVertexArray(0);
}

void Terrain::generateNormals(std::vector<Vertex>& Vertices) const
{
	// Initialize normals to zero
	for (auto& Vertex : Vertices)
	{
		Vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	// For each vertex calculate normal by averaging normals of adjacent faces
	for (unsigned int Z = 0; Z < PvTerrainInfo.Depth - 1; ++Z)
	{
		for (unsigned int X = 0; X < PvTerrainInfo.Width - 1; ++X)
		{
			// Indices of four corners of current quad
			unsigned int TopLeft = Z * PvTerrainInfo.Width + X;
			unsigned int TopRight = TopLeft + 1;
			unsigned int BottomLeft = (Z + 1) * PvTerrainInfo.Width + X;
			unsigned int BottomRight = BottomLeft + 1;

			// Vertices of four corners
			glm::vec3 Vtl = Vertices[TopLeft].Position;
			glm::vec3 Vtr = Vertices[TopRight].Position;
			glm::vec3 Vbl = Vertices[BottomLeft].Position;
			glm::vec3 Vbr = Vertices[BottomRight].Position;

			// Calculate normal for first triangle (TL, BL, TR)
			glm::vec3 V1 = Vbl - Vtl;
			glm::vec3 V2 = Vtr - Vtl;
			glm::vec3 Normal1 = normalize(cross(V1, V2));

			// Calculate normal for second triangle (TR, BL, BR)
			glm::vec3 V3 = Vbr - Vtr;
			glm::vec3 V4 = Vbl - Vtr;
			glm::vec3 Normal2 = normalize(cross(V3, V4));

			// Add normals to each involved vertices
			// Normalize after all contributions are summed
			Vertices[TopLeft].Normal += Normal1;
			Vertices[TopRight].Normal += Normal1;
			Vertices[TopRight].Normal += Normal2;
			Vertices[BottomLeft].Normal += Normal1;
			Vertices[BottomLeft].Normal += Normal2;
			Vertices[BottomRight].Normal += Normal2;
		}
	}

	// Normalize all normals
	for (auto& Vertex : Vertices)
	{
		if (length(Vertex.Normal) > 0.0f)
		{
			Vertex.Normal = normalize(Vertex.Normal);
		}
		else
		{
			// Default normal pointing upward if no face contributions
			Vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
		}
	}
}

void Terrain::setupIndexBuffer()
{
	const unsigned int FaceCount = (PvTerrainInfo.Width - 1) * (PvTerrainInfo.Depth - 1) * 2;
	const unsigned int DrawCount = FaceCount * 3;
	std::vector<GLuint> Indices(DrawCount);

	int Index = 0;
	for (unsigned int Row = 0; Row < PvTerrainInfo.Depth - 1; Row++)
	{
		for (unsigned int Col = 0; Col < PvTerrainInfo.Width - 1; Col++)
		{
			// First triangle
			Indices[Index++] = Row * PvTerrainInfo.Width + Col; // Bottom left
			Indices[Index++] = Row * PvTerrainInfo.Width + (Col + 1); // Bottom right
			Indices[Index++] = (Row + 1) * PvTerrainInfo.Width + Col; // Top left

			// Second triangle
			Indices[Index++] = Row * PvTerrainInfo.Width + (Col + 1); // Bottom right
			Indices[Index++] = (Row + 1) * PvTerrainInfo.Width + (Col + 1); // Top right
			Indices[Index++] = (Row + 1) * PvTerrainInfo.Width + Col; // Top left
		}
	}

	glGenBuffers(1, &PvEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, PvEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long long>(Indices.size() * sizeof(GLuint)), Indices.data(),
	             GL_STATIC_DRAW);
}

void Terrain::drawTerrain() const
{
	GLboolean CullFaceEnabled;
	GLint CullFaceMode;
	glGetBooleanv(GL_CULL_FACE, &CullFaceEnabled);
	glGetIntegerv(GL_CULL_FACE_MODE, &CullFaceMode);

	glDisable(GL_CULL_FACE);

	glBindVertexArray(PvVao);

	glDrawElements(GL_TRIANGLES, static_cast<int>((PvTerrainInfo.Width - 1) * (PvTerrainInfo.Depth - 1)) * 6,
	               GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);

	if (CullFaceEnabled)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(CullFaceMode);
	}
}
