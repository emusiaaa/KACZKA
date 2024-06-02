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
	void rain() {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis1(0, 1.f);
		std::uniform_int_distribution<> dis(0, 255);
		float p = dis1(gen);
		if(p > 0.85f)
		{
			int ii = dis(gen);
			int jj = dis(gen);
			Z_N[ii][jj] = -1.f;
		}
	}
	void disturb(int ii, int jj, unsigned int& tex) {
		rain();
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
		createNormalTexture(calculateNormals(), tex);
		/*std::vector<unsigned char> image = convertHeightMapToImage();
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, N, N, 0, GL_RED, GL_UNSIGNED_BYTE, image.data());
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);*/
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

	std::vector<std::vector<glm::vec3>> calculateNormals() {
		static_assert(N > 1, "N too smol");
		auto normals = std::vector<std::vector<glm::vec3>>(N, std::vector<glm::vec3>(N));
		for (int y = 0; y < N; ++y) {
			normals[y][0].x = (Z_N_plus_1[y][0] + Z_N_plus_1[y][1]) / 4.0f;
			for (int x = 1; x < N - 1 ; ++x) {
				normals[y][x].x = (Z_N_plus_1[y][x - 1] + Z_N_plus_1[y][x + 1]) / 2.0f;
			}
			normals[y][N - 1].x = (Z_N_plus_1[y][N - 2] + Z_N_plus_1[y][N - 1]) / 4.0f;
		}
		for (int x = 0; x < N; ++x) {
			normals[0][x].z = (Z_N_plus_1[0][x] + Z_N_plus_1[1][x]) / 4.0f;
			for (int y = 1; y < N - 1; ++y) {
				normals[y][x].z = (Z_N_plus_1[y - 1][x] + Z_N_plus_1[y + 1][x]) / 2.0f;
			}
			normals[0][x].z = (Z_N_plus_1[N - 2][x] + Z_N_plus_1[N - 1][x]) / 4.0f;
		}
		for (int x = 0; x < N; ++x) {
			for (int y = 0; y < N; ++y) {
				normals[y][x].y = 1.0f;
			}
		}
		for (auto& vecs : normals) {
			for (auto& vec : vecs) {
				const float mul = 1.0f / std::sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
				vec.x *= mul;
				vec.y *= mul;
				vec.z *= mul;
				std::swap(vec.y, vec.z);
			}
		}
		return normals;
	}

	glm::u8vec3 encodeNormal(const glm::vec3& normal) {
		glm::u8vec3 encoded;
		encoded.r = static_cast<unsigned char>((normal.x + 1.0f) * 0.5f * 255.0f);
		encoded.g = static_cast<unsigned char>((normal.y + 1.0f) * 0.5f * 255.0f);
		encoded.b = static_cast<unsigned char>((normal.z + 1.0f) * 0.5f * 255.0f);
		return encoded;
	}

	void createNormalTexture(const std::vector<std::vector<glm::vec3>>& normals, unsigned int& tex) {
		int height = normals.size();
		int width = normals[0].size();

		std::vector<glm::u8vec3> normalMap(width * height);
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				int index = (y * N + x) * 4;
				normalMap[y * width + x] = encodeNormal(normals[y][x]);
			}
		}

		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, normalMap.data());
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
};