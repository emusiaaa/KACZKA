#pragma once
#include <glm/glm.hpp>
#include <random>
#include "ShaderProgram.h"

class PathMaker {
public:
	glm::vec3 points[4];
	glm::vec3 berstein[4];
	glm::vec3 f1, f2;
	glm::vec3 g1, g2;
	glm::vec3 e1, e2;
	float WIDTH;
	ShaderProgram shader;
	unsigned int VAO, VBO;

	PathMaker(float width = 250.f): WIDTH(width) {
		shader = ShaderProgram("basic.vert", "basic.frag");
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		points[0] = glm::vec3(-200, 0, 200);
		points[1] = glm::vec3(-100, 0, -200);
		points[2] = glm::vec3(100, 0, -200);
		points[3] = glm::vec3(200, 0, 200);

		g1 = (1.0f / 3.0f) * points[0] + (2.0f / 3.0f) * points[1];
		f1 = (2.0f / 3.0f) * points[1] + (1.0f / 3.0f) * points[2];
		f2 = (2.0f / 3.0f) * points[2] + (1.0f / 3.0f) * points[3];
		g2 = (1.0f / 3.0f) * points[1] + (2.0f / 3.0f) * points[2];
		e1 = 0.5f * g1 + 0.5f * f1;
		e2 = 0.5f * g2 + 0.5f * f2;

		berstein[0] = e1;
		berstein[1] = f1;
		berstein[2] = g2;
		berstein[3] = e2;

		std::vector<float> vertices;
		for (const auto& point : berstein) {
			vertices.push_back(point.x);
			vertices.push_back(point.y);
			vertices.push_back(point.z);
		}

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

	glm::vec3 calculateCurrentPosition(float& t) {
		if (t > 1.0f) {
			t = std::fmod(t, 1.0f);
			update();
		}
		glm::vec3 p[4] = { berstein[0], berstein[1], berstein[2], berstein[3] };
		for (int i = 4; i > 1; --i)
		{
			for (int j = 0; j < i - 1; ++j) {
				p[j] = p[j] * (1.0f - t) + p[j + 1] * t;
			}
		}
		return p[0];
	}
	void update() {
		points[0] = points[1];
		points[1] = points[2];
		points[2] = points[3];
		points[3] = randomPoint();

		g1 = g2;
		f1 = f2;
		e1 = e2;

		f2 = (2.0f / 3.0f) * points[2] + (1.0f / 3.0f) * points[3];
		g2 = (1.0f / 3.0f) * points[1] + (2.0f / 3.0f) * points[2];
		e2 = 0.5f * g2 + 0.5f * f2;

		berstein[0] = e1;
		berstein[1] = f1;
		berstein[2] = g2;
		berstein[3] = e2;

		std::vector<float> vertices;
		for (const auto& point : berstein) {
			vertices.push_back(point.x);
			vertices.push_back(point.y);
			vertices.push_back(point.z);
		}

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}
	glm::vec3 bezierTangent(float t) {
		glm::vec3 P0 = berstein[0];
		glm::vec3 P1 = berstein[1];
		glm::vec3 P2 = berstein[2];
		glm::vec3 P3 = berstein[3];

		float u = 1.0f - t;
		glm::vec3 tangent =
			3.0f * u * u * (P1 - P0) +
			6.0f * u * t * (P2 - P1) +
			3.0f * t * t * (P3 - P2);

		return glm::normalize(tangent);
	}
	glm::mat4 alignModelToVector(const glm::vec3 targetDirection) {
		glm::vec3 initialDirection = glm::vec3(-1.0f, 0.0f, 0.0f); // Initial direction

		glm::vec3 axis = glm::cross(initialDirection, targetDirection);
		float angle = acos(glm::dot(initialDirection, targetDirection));

		// Handle the case where the vectors are already aligned or opposite
		if (glm::length(axis) < 0.0001f) {
			if (glm::dot(initialDirection, targetDirection) > 0.0f) {
				// They are already aligned
				return glm::mat4(1.0f);
			}
			else {
				// They are opposite
				return glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
			}
		}

		glm::quat quaternion = glm::angleAxis(angle, glm::normalize(axis));
		glm::mat4 rotationMatrix = glm::toMat4(quaternion);

		return rotationMatrix;
	}
	glm::vec3 randomPoint() {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(-WIDTH, WIDTH);

		float x = dis(gen);
		float z = dis(gen);
		float y = 0.0f;

		return glm::vec3(x, y, z);
	}
	void draw(glm::mat4 view, glm::mat4 projection) {
		glBindVertexArray(VAO);
		shader.use();
		shader.setMat4("model", glm::mat4(1));
		shader.setMat4("view", view);
		shader.setMat4("proj", projection);
		shader.setVec3("color", 1.f, 0.f, 0.f);
		
		glDrawArrays(GL_LINE_STRIP, 0, 4);
	}
};


