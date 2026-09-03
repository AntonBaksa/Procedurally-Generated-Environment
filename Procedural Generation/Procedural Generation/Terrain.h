#pragma once

#include <vector>

struct Vertex
{
	float x;
	float y;
	float z;
};

class Terrain
{
public:
	int size = 32;
	float maxHeight = 1.0f;

	float noiseFrequency = 0.01;

	std::vector<float> heightmap;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	void CreateNoise();
	void CreateMesh();
};

