#include "Plane.h"

using namespace glm;

namespace PUMA
{

	Plane::Plane(const float& size)
		: Plane(size, size)
	{ }

	Plane::Plane(const float& width, const float& height)
	{
		InitGL(width, height);
	}

	Plane::~Plane()
	{
		glDeleteVertexArrays(1, &m_gl_VAO);
		glDeleteBuffers(1, &m_gl_VBO);
		glDeleteBuffers(1, &m_gl_EBO);
	}

	void Plane::Draw()
	{
		glBindVertexArray(m_gl_VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}

	std::vector<vec3> Plane::GenerateVerticesNormals(const float& width, const float& height)
	{
		// The Plane lies on XZ plane

		return {
			// Vertices
			{  width / 2, 0.0f,  height / 2 },	// 0
			{ -width / 2, 0.0f,  height / 2 },	// 1
			{ -width / 2, 0.0f, -height / 2 },	// 2
			{  width / 2, 0.0f, -height / 2 },	// 3

			// Normals
			{ 0.f, 1.f, 0.f},	// 0
			{ 0.f, 1.f, 0.f},	// 1
			{ 0.f, 1.f, 0.f},	// 2
			{ 0.f, 1.f, 0.f},	// 3
		};
	}

	std::vector<unsigned int> Plane::GenerateIndices()
	{
		return {
			2, 1, 0,
			0, 3, 2
		};
	}

	void Plane::InitGL(const float& width, const float& height)
	{
		// Generate buffers
		glGenVertexArrays(1, &m_gl_VAO);
		glGenBuffers(1, &m_gl_VBO);
		glGenBuffers(1, &m_gl_EBO);

		// Bind buffers to VAO and populate them
		std::vector<vec3>			vertNorm	= GenerateVerticesNormals(width, height);
		std::vector<unsigned int>	indices		= GenerateIndices();

		glBindVertexArray(m_gl_VAO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
		
		glBindBuffer(GL_ARRAY_BUFFER, m_gl_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertNorm.size() * sizeof(vec3), vertNorm.data(), GL_STATIC_DRAW);
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat) * 4));
		
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		// Unbind everything from context

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

} // PUMA