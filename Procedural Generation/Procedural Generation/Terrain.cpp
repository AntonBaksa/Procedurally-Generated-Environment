#include "Terrain.h"
#include "FastNoiseLite.h"
#include <vector>

void CreateNoise() 
{
	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

	std::vector<float> noiseData(area * area);
	int index = 0;

	for (int y = 0; y < area; y++) {
		for (int x = 0; x < area; x++) {
			noiseData[index++] = noise.GetNoise((float)x, (float)y);
		}
	}
}