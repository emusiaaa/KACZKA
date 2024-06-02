#pragma once

// Includes (libs)
#include <glad/glad.h>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

// Includes (prog)

// Forward Declarations

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=

namespace PUMA
{

	class Plane
	{
		private:

			// *=*=*=*=*=*=*=*=*=*=*=*
			//     OpenGL things
			// *=*=*=*=*=*=*=*=*=*=*=*

			GLuint m_gl_VAO;
			GLuint m_gl_VBO;
			GLuint m_gl_TBO;
			GLuint m_gl_EBO;

		public:
			
			// *=*=*=*=*=*=*=*=*=*=*=*
			// Constructors/Destructor
			// *=*=*=*=*=*=*=*=*=*=*=*

			Plane(const float& size);
			Plane(const float& width, const float& height);

			~Plane();

			void Draw();
			void Draw(unsigned int& texture, GLenum textureType = GL_TEXTURE_CUBE_MAP) const;

		private:

			std::vector<glm::vec3> GenerateVerticesNormals(const float& width, const float& height);
			std::vector<glm::vec2> GenerateTexCoords();
			std::vector<unsigned int> GenerateIndices();

			void InitGL(const float& width, const float& height);
	};

} // PUMA

