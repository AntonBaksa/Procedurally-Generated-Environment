#include "Terrain.h"
#include "FastNoiseLite.h"
#include <cmath>

void Terrain::CreateNoise() 
{
	heightmap.resize(size * size);

	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	noise.SetFrequency(noiseFrequency);

	for (int y = 0; y < size; y++) 
	{
		for (int x = 0; x < size; x++) 
		{
			float noiseValue = noise.GetNoise((float)x, (float)y);
			
			float normalized = (noiseValue + 1.0f) * 0.5f; // Convert [-1, 1] to [0, 1]

			heightmap[x + y * size] = normalized * maxHeight;
		}
	}
}

void Terrain::CreateMesh()
{

	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			float height = heightmap[x + y * size];

			Vertex vertex;
			vertex.x = x;
			vertex.y = height;
			vertex.z = y;

			vertices.push_back(vertex);
		}
	}

	for (int y = 0; y < size - 1; y++)
	{
		for (int x = 0; x < size - 1; x++)
		{
			//rectangle
			int topLeft = x + y * size;
			int topRight = topLeft + 1;
			int bottomLeft = topLeft + size;
			int bottomRight = topRight + size;

			//triangle 1
			indices.push_back(topLeft);
			indices.push_back(bottomLeft);
			indices.push_back(topRight);

			//triangle 2
			indices.push_back(topRight);
			indices.push_back(bottomLeft);
			indices.push_back(bottomRight);
		}
	}
	CreateNormals();
}

void Terrain::CreateNormals()
{
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		Vertex& v0 = vertices[indices[i]];
		Vertex& v1 = vertices[indices[i + 1]];
		Vertex& v2 = vertices[indices[i + 2]];

		// Two edges of the triangle
		float e1x = v1.x - v0.x, e1y = v1.y - v0.y, e1z = v1.z - v0.z;
		float e2x = v2.x - v0.x, e2y = v2.y - v0.y, e2z = v2.z - v0.z;

		// The cross product of two edges
		float nx = e1y * e2z - e1z * e2y;
		float ny = e1z * e2x - e1x * e2z;
		float nz = e1x * e2y - e1y * e2x;

		//Add face normal to all 3 vertices
		v0.nx += nx; v0.ny += ny; v0.nz += nz;
		v1.nx += nx; v1.ny += ny; v1.nz += nz;
		v2.nx += nx; v2.ny += ny; v2.nz += nz;
	}

	for (Vertex& v : vertices)
	{
		float length = std::sqrt(v.nx * v.nx + v.ny * v.ny + v.nz * v.nz);
		if (length > 0.0f)
		{
			v.nx /= length;
			v.ny /= length;
			v.nz /= length;
		}
	}
}