#include "noise.h"

float Random(int x, int y) 
{
	int n = x + y * 57;
	n = (n << 13) ^ n;
	int t = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
	return 1.0 - double(t) * 0.931322574615478515625e-9;
}

float Smooth(int x, int y)
{
	float corners;
	float sides;
	float center;
	corners = (Random(x - 1, y - 1) + Random(x + 1, y - 1) + Random(x - 1, y + 1) + Random(x + 1, y + 1)) / 16;
	sides = (Random(x - 1, y) + Random(x + 1, y) + Random(x, y - 1) + Random(x, y + 1)) / 8;
	center = Random(x, y) / 4;
	return corners + sides + center;
}

float Interpolate(float a, float b, float x) 
{
	return a * (1 - x) + b * x;
}

float Noise(float x, float y) 
{
	float fractional_X = x - int(x);
	float fractional_Y = y - int(y);
	float final;

	//smooths
	float v1 = Smooth(int(x), int(y));
	float v2 = Smooth(int(x) + 1, int(y));
	float v3 = Smooth(int(x), int(y) + 1);
	float v4 = Smooth(int(x) + 1, int(y) + 1);

	// interpolates
	float i1 = Interpolate(v1, v2, fractional_X);
	float i2 = Interpolate(v3, v4, fractional_X);
	return final = Interpolate(i1, i2, fractional_Y);
}

float TotalNoisePerPoint(int x, int y) 
{
	int octaves = 8;
	float zoom = 20.0f;
	float persistence = 0.5f;
	float total = 0.0f;

	for (int i = 0; i < octaves - 1; i++) 
	{
		float frequency = pow(2, i) / zoom;
		float amplitude = pow(persistence, i);
		total += Noise(x * frequency, y * frequency) * amplitude;
	}

	return total;
}