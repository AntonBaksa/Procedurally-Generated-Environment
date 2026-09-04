#pragma once

#include <vector>

struct Vertex
{
	float x;
	float y;
	float z;

	float nx = 0.0f;
	float ny = 0.0f;
	float nz = 0.0f;
};

class Terrain
{
public:
	int size = 32;
	float maxHeight = 1.0f;
	float noiseFrequency = 0.6f;

	std::vector<float> heightmap;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	void CreateNoise();
	void CreateMesh();
	void CreateNormals();
};

