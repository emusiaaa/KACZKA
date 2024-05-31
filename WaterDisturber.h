#pragma once
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_PERLIN_IMPLEMENTATION

#include <glad/glad.h>
#include <vector>
#include <iostream>
#include <glm/vec3.hpp>
#include <random>
#include "3rd/stb_image/stb_image_write.h"
#include "3rd/stb_perlin.h"

class WaterDisturber
{
public:
	const int WIDTH = 1512;
	const int HEIGHT = 1512;
	const float SCALE = 0.1f;

	static const int N = 256;
	const float h = 2.0f / (static_cast<float>(N) - 1.0f);
	const float c = 1.0f;
	const float dt = 1.0f / static_cast<float>(N);
	const float A = (c * c * dt * dt) / (h * h);
	const float B = 2.0f - 4.0f * A;

	std::vector<std::vector<float>> Z_N;
	std::vector<std::vector<float>> Z_N_1;
	std::vector<std::vector<float>> Z_N_plus_1;
	std::vector<std::vector<float>> d;

	WaterDisturber() {
		Z_N.resize(N, std::vector<float>(N));
		Z_N_1.resize(N, std::vector<float>(N));
		Z_N_plus_1.resize(N, std::vector<float>(N));
		d.resize(N, std::vector<float>(N, 0.95f));
	};
	~WaterDisturber() {};

	std::vector<float> generateHeightMap(int width, int height, float scale) {
		std::vector<float> heightMap(width * height);
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				float nx = x * scale;
				float ny = y * scale;
				heightMap[y * width + x] = stb_perlin_noise3(nx, ny, 0.0f, 0, 0, 0);
			}
		}
		return heightMap;
	}

	std::vector<unsigned char> convertHeightMapToImage(const std::vector<float>& heightMap, int width, int height) {
		std::vector<unsigned char> image(width * height);
		for (int i = 0; i < width * height; ++i) {
			// Normalize from [-1, 1] to [0, 255]
			float normalizedHeight = (heightMap[i] + 1.0f) / 2.0f;
			image[i] = static_cast<unsigned char>(normalizedHeight * 255);
		}
		return image;
	}

	void calculateDampingCoefficients() {
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				float distance = std::min({static_cast<float>(i), static_cast<float>(N - 1 - i),
										   static_cast<float>(j), static_cast<float>(N - 1 - j) });
				d[i][j] = 0.95f * std::min(1.0f, distance / 2.0f);
			}
		}
	}

	void disturb() {
		std::vector<float> heightMap = generateHeightMap(WIDTH, HEIGHT, SCALE);
		std::vector<unsigned char> image = convertHeightMapToImage(heightMap, WIDTH, HEIGHT);

		// Save the image as a PNG file
		if (stbi_write_png("heightmap.png", WIDTH, HEIGHT, 1, image.data(), WIDTH) == 0) {
			std::cout << "Failed to write image" << std::endl;
			return;
		}

		std::cout << "Height map saved as heightmap.png" << std::endl;
	}

	void disturb(int ii, int jj, unsigned int& tex) {
		Z_N[ii][jj] = -1.f;
		for (int i = 1; i < N - 1; i++) {
			for (int j = 1; j < N - 1; j++) {
				Z_N_plus_1[i][j] = d[i][j] * (A * (Z_N[i + 1][j] + Z_N[i-1][j] + Z_N[i][j-1] + Z_N[i][j + 1]) + B * Z_N[i][j] - Z_N_1[i][j]);
			}
		}
		
		for (size_t i = 0; i < N; ++i) {
			for (size_t j = 0; j < N; ++j) {
				Z_N_1[i][j] = Z_N[i][j];
				Z_N[i][j] = Z_N_plus_1[i][j];
			}
		}
		
		std::vector<unsigned char> image = convertHeightMapToImage();
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, N, N, 0, GL_RED, GL_UNSIGNED_BYTE, image.data());
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		/*if (stbi_write_png("heightmap2.png", N, N, 1, image.data(), N) == 0) {
			std::cout << "Failed to write image" << std::endl;
			return;
		}*/

	}
	void disturb2(unsigned int& tex) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, 255);
		int ii = dis(gen);
		int jj = dis(gen);
		Z_N[ii][jj] = -1.f;

		for (int i = 1; i < N - 1; i++) {
			for (int j = 1; j < N - 1; j++) {
				Z_N_plus_1[i][j] = d[i][j] * (A * (Z_N[i + 1][j] + Z_N[i - 1][j] + Z_N[i][j - 1] + Z_N[i][j + 1]) + B * Z_N[i][j] - Z_N_1[i][j]);
			}
		}
		for (size_t i = 0; i < N; ++i) {
			for (size_t j = 0; j < N; ++j) {
				Z_N_1[i][j] = Z_N[i][j];
				Z_N[i][j] = Z_N_plus_1[i][j];
			}
		}
		std::vector<unsigned char> image = convertHeightMapToImage();
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, N, N, 0, GL_RED, GL_UNSIGNED_BYTE, image.data());
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	std::vector<unsigned char> convertHeightMapToImage() {
		std::vector<unsigned char> image(N * N);
		for (int y = 0; y < N; ++y) {
			for (int x = 0; x < N; ++x) {
				// Normalize from [-1, 1] to [0, 255]
				float normalizedHeight = (Z_N_plus_1[y][x] + 1.0f) / 2.0f;
				image[y * N + x] = static_cast<unsigned char>(normalizedHeight * 255);
			}
		}
		return image;
	}
};