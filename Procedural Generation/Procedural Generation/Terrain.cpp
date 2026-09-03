#include "Terrain.h"
#include "FastNoiseLite.h"

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
			int topLeft = x + y * size;
			int topRight = topLeft + 1;
			int bottomLeft = topLeft + size;
			int bottomRight = topRight + size;

			//triangle 1
			indices.push_back(topLeft);
			indices.push_back(topRight);
			indices.push_back(bottomLeft);

			//triangle 2
			indices.push_back(topRight);
			indices.push_back(bottomRight);
			indices.push_back(bottomLeft);
		}
	}
}