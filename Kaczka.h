#pragma once

// Includes (libs)
#include <glad/glad.h>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

// Includes (prog)
#include "Matrix.h"

// Forward Declarations

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=

namespace KACZKA
{
	class Kaczka
	{
		private:

			struct Triangle
			{
				unsigned int A;
				unsigned int B;
				unsigned int C;
			};

		private:

			// *=*=*=*=*=*=*=*=*=*=*=*
			//     Mesh elemenets
			// *=*=*=*=*=*=*=*=*=*=*=*

			std::vector<glm::vec3>		m_mesh_vertexPositions;
			std::vector<glm::vec3>		m_mesh_normals;
			std::vector<glm::vec2>		m_mesh_texCoords;
			std::vector<Triangle>		m_mesh_triangles;

			// *=*=*=*=*=*=*=*=*=*=*=*
			//     OpenGL things
			// *=*=*=*=*=*=*=*=*=*=*=*

			GLuint m_gl_VAO;
			GLuint m_gl_VBO; // Vertex Buffer Object
			GLuint m_gl_NBO; // Normal Buffer Object
			GLuint m_gl_TBO; // Texture Buffer Object
			GLuint m_gl_EBO;  

		public:
			float scale = 0.5f;
			glm::vec3 translation = glm::vec3(0);

		public:

			// *=*=*=*=*=*=*=*=*=*=*=*
			// Constructors/Destructor
			// *=*=*=*=*=*=*=*=*=*=*=*

			Kaczka(const char* path);
			~Kaczka();

			void Draw(unsigned int& texture) const;
			glm::mat4 ModelMtx() {
				return Translate(translation.x, translation.y, translation.z) * Scale(scale);
			}

		private:

			void ReadMeshData(const char* filePath);
			void InitGL();
	};

}