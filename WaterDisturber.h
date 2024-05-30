#pragma once

#include <glad/glad.h>
#include <vector>

#include <glm/vec3.hpp>

class WaterDisturber
{
public:
	int height;
	WaterDisturber(int height): height(height) {};
	~WaterDisturber() {};
};