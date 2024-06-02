#include "Kaczka.h"

// Includes
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=

namespace KACZKA
{

	Kaczka::Kaczka(const char* path) 
	{
		ReadMeshData(path);

		InitGL();
	}

	Kaczka::~Kaczka()
	{
		std::cout << "~ KACZKA" << std::endl;
		glDeleteVertexArrays(1, &m_gl_VAO);
		glDeleteBuffers(1, &m_gl_VBO);
		glDeleteBuffers(1, &m_gl_NBO);
		glDeleteBuffers(1, &m_gl_TBO);
		glDeleteBuffers(1, &m_gl_TGBO);
		glDeleteBuffers(1, &m_gl_EBO);
	}

	void Kaczka::Draw(unsigned int& texture) const
	{
		glBindVertexArray(m_gl_VAO);
		glBindTexture(GL_TEXTURE_2D, texture);
		glDrawElements(GL_TRIANGLES, 3 * (GLsizei)m_mesh_triangles.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Kaczka::ReadMeshData(const char* filePath) {
		std::ifstream inputFile(filePath);

		if (!inputFile) {
			std::cerr << "Error opening file!" << std::endl;
			return;
		}

		std::string line;
		std::string segment;
		
		unsigned int V, T;

		std::getline(inputFile, line);
		std::istringstream iss_positions(line);
		iss_positions >> V;
		m_mesh_vertexPositions.reserve(V);
		m_mesh_normals.reserve(V);
		m_mesh_texCoords.reserve(V);

		// Populate Buffer
		glm::vec3 position, normal;
		glm::vec3 tex;
		for (unsigned int i = 0; i < V; i++) {
			std::getline(inputFile, line);
			std::istringstream iss(line);
			for (int j = 0; j < 3; j++) {
				iss >> segment;
				position[j] = std::stof(segment);
			}
			m_mesh_vertexPositions.push_back(position);
			for (int j = 0; j < 3; j++) {
				iss >> segment;
				normal[j] = std::stof(segment);
			}
			m_mesh_normals.push_back(normal);
			for (int j = 0; j < 2; j++) {
				iss >> segment;
				tex[j] = std::stof(segment);
			}
			m_mesh_texCoords.push_back(tex);
		}

		// m_mesh_triangles		   //
		// *=*=*=*=*=*=*=*=*=*=*=* //

		std::getline(inputFile, line);
		std::istringstream iss_triangles(line);
		iss_triangles >> T;
		m_mesh_triangles.reserve(T);

		// Populate Buffer
		Triangle tr;
		for (unsigned int i = 0; i < T; i++) {
			std::getline(inputFile, line);
			std::istringstream iss(line);

			iss >> segment;
			tr.A = std::stoi(segment);
			iss >> segment;
			tr.B = std::stoi(segment);
			iss >> segment;
			tr.C = std::stoi(segment);

			m_mesh_triangles.push_back(tr);
		}

		inputFile.close();

		m_mesh_tangents.resize(m_mesh_vertexPositions.size());
		for (int i = 0; i < m_mesh_triangles.size(); ++i) {
			auto t = m_mesh_triangles[i];
			auto v0 = m_mesh_vertexPositions[t.A];
			auto v1 = m_mesh_vertexPositions[t.B];
			auto v2 = m_mesh_vertexPositions[t.C];

			auto t0 = m_mesh_texCoords[t.A];
			auto t1 = m_mesh_texCoords[t.B];
			auto t2 = m_mesh_texCoords[t.C];

			glm::vec3 edge1 = v1 - v0;
			glm::vec3 edge2 = v2 - v0;
			glm::vec2 deltaUV1 = t1 - t0;
			glm::vec2 deltaUV2 = t2 - t0;

			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			glm::vec3 tangent;

			tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

			m_mesh_tangents[t.A] += tangent;
			m_mesh_tangents[t.B] += tangent;
			m_mesh_tangents[t.C] += tangent;
		}
		for (int i = 0; i < m_mesh_tangents.size(); ++i) {
			m_mesh_tangents[i] = glm::normalize(m_mesh_tangents[i]);
		}
	}

	void Kaczka::InitGL()
	{
		// Create buffers and vertex array 
		glGenVertexArrays(1, &m_gl_VAO);
		glGenBuffers(1, &m_gl_VBO);
		glGenBuffers(1, &m_gl_NBO);
		glGenBuffers(1, &m_gl_TBO);
		glGenBuffers(1, &m_gl_TGBO);
		glGenBuffers(1, &m_gl_EBO);

		// Populate buffers with data
		glBindBuffer(GL_ARRAY_BUFFER, m_gl_VBO);
		glBufferData(GL_ARRAY_BUFFER, m_mesh_vertexPositions.size() * sizeof(glm::vec3), m_mesh_vertexPositions.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, m_gl_NBO);
		glBufferData(GL_ARRAY_BUFFER, m_mesh_normals.size() * sizeof(glm::vec3), m_mesh_normals.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, m_gl_TBO);
		glBufferData(GL_ARRAY_BUFFER, m_mesh_texCoords.size() * sizeof(glm::vec2), m_mesh_texCoords.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, m_gl_TGBO);
		glBufferData(GL_ARRAY_BUFFER, m_mesh_tangents.size() * sizeof(glm::vec3), m_mesh_tangents.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, m_gl_EBO);
		glBufferData(GL_ARRAY_BUFFER, m_mesh_triangles.size() * sizeof(Triangle), m_mesh_triangles.data(), GL_STATIC_DRAW);

		// Bind buffers to VAO
		glBindVertexArray(m_gl_VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_EBO);
		
		glBindBuffer(GL_ARRAY_BUFFER, m_gl_VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, m_gl_NBO);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, m_gl_TBO);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, m_gl_TGBO);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		// Unbind everything 
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

} // PUMA