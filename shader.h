#pragma once
#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	unsigned int id;
	const char* path;

	Shader(): path(""), id(0) {}
	Shader(const char* path) : path(path), id(0) {}

	virtual void init() = 0;

	void deleteShader()
	{
		glDeleteShader(id);
	}
protected:
	std::string readShader()
	{
		std::string code;
		std::ifstream shader_file;
		shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			shader_file.open(path);
			std::stringstream shader_stream;
			shader_stream << shader_file.rdbuf();
			shader_file.close();
			code = shader_stream.str();
		}
		catch (std::ifstream::failure& e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
		}
		return code;
	}
	void source_and_compile(const char* code) 
	{
		glShaderSource(id, 1, &code, NULL);
		glCompileShader(id);
	}
	void checkCompileErrors(std::string type)
	{
		int success;
		char infoLog[1024];

		glGetShaderiv(id, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(id, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
};

class VertexShader: public Shader {
public:
	VertexShader() : Shader() {}
	VertexShader(const char* path) : Shader(path) {}

	void init() override {
		std::string code = readShader();
		id = glCreateShader(GL_VERTEX_SHADER);
		source_and_compile(code.c_str());
		checkCompileErrors("VERTEX");
	}
};

class FragmentShader : public Shader {
public:
	FragmentShader() : Shader() {}
	FragmentShader(const char* path) : Shader(path) {}

	void init() override {
		std::string code = readShader();
		id = glCreateShader(GL_FRAGMENT_SHADER);
		source_and_compile(code.c_str());
		checkCompileErrors("FRAGMENT");
	}
};
