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
		glDeleteBuffers(1, &m_gl_EBO);
	}

	Kaczka::Kaczka(Kaczka&& moved) noexcept
		: 
		m_mesh_vertexPositions(std::move(moved.m_mesh_vertexPositions)),
		m_mesh_normals(std::move(moved.m_mesh_normals)),
		m_mesh_triangles(std::move(moved.m_mesh_triangles))
	{
		m_gl_VAO = moved.m_gl_VAO;
		m_gl_VBO = moved.m_gl_VBO;
		m_gl_NBO = moved.m_gl_NBO;
		m_gl_EBO = moved.m_gl_EBO;

		moved.m_gl_VAO = moved.m_gl_VBO = moved.m_gl_NBO = moved.m_gl_EBO = 0;
	}

	Kaczka& Kaczka::operator=(Kaczka&& moved) noexcept
	{
		m_mesh_vertexPositions	= std::move(moved.m_mesh_vertexPositions);
		m_mesh_normals			= std::move(moved.m_mesh_normals);
		m_mesh_triangles		= std::move(moved.m_mesh_triangles);

		m_gl_VAO = moved.m_gl_VAO;
		m_gl_VBO = moved.m_gl_VBO;
		m_gl_NBO = moved.m_gl_NBO;
		m_gl_EBO = moved.m_gl_EBO;

		moved.m_gl_VAO = moved.m_gl_VBO = moved.m_gl_NBO = moved.m_gl_EBO = 0;

		return *this;
	}

	void Kaczka::Draw(unsigned int& texture) const
	{
		glBindVertexArray(m_gl_VAO);
		glBindTexture(GL_TEXTURE_2D, texture);
		glDrawElements(GL_TRIANGLES, 3 * (GLsizei)m_mesh_triangles.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
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
	}

	void Kaczka::InitGL()
	{
		// Create buffers and vertex array 
		glGenVertexArrays(1, &m_gl_VAO);
		glGenBuffers(1, &m_gl_VBO);
		glGenBuffers(1, &m_gl_NBO);
		glGenBuffers(1, &m_gl_TBO);
		glGenBuffers(1, &m_gl_EBO);

		// Populate buffers with data
		glBindBuffer(GL_ARRAY_BUFFER, m_gl_VBO);
		glBufferData(GL_ARRAY_BUFFER, m_mesh_vertexPositions.size() * sizeof(glm::vec3), m_mesh_vertexPositions.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, m_gl_NBO);
		glBufferData(GL_ARRAY_BUFFER, m_mesh_normals.size() * sizeof(glm::vec3), m_mesh_normals.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, m_gl_TBO);
		glBufferData(GL_ARRAY_BUFFER, m_mesh_texCoords.size() * sizeof(glm::vec2), m_mesh_texCoords.data(), GL_STATIC_DRAW);

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

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		// Unbind everything 
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

} // PUMA