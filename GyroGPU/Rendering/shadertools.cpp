#include "shadertools.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

#include <GL/glew.h>


GLuint CompileShader(std::string shader_source_path, GLuint type) {
	// initialiize shader
	GLuint shader = glCreateShader(type);

	// read string from file
	std::ifstream file(shader_source_path);
	std::string shader_source((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>()); // don't ask me how this works. https://stackoverflow.com/questions/18398167/how-to-copy-a-txt-file-to-a-char-array-in-c
	file.close();

	// make the data type something stupid and c-based.
	const char* shader_source_c = shader_source.c_str();

	// compile shader
	glShaderSource(shader, 1, &shader_source_c, nullptr);
	glCompileShader(shader);

	int result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

	if (result == GL_TRUE) {
		return shader;
	}

	// if we didn't just return, something fucked up. Let's print some error messages.
	int length;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
	char* errors = new char[length];
	glGetShaderInfoLog(shader, length, &length, errors);

	switch (type) {
	case GL_VERTEX_SHADER:
		std::cout << "ERROR: GL_VERTEX_SHADER compilation failed!" << std::endl << errors << std::endl;
		break;
	case GL_FRAGMENT_SHADER:
		std::cout << "ERROR: GL_FRAGMENT_SHADER compilation failed!" << std::endl << errors << std::endl;
		break;
	case GL_COMPUTE_SHADER:
		std::cout << "ERROR: GL_COMPUTE_SHADER compilation failed!" << std::endl << errors << std::endl;
		break;
	default:
		std::cout << "ERROR: shader compilation failed!" << std::endl << errors << std::endl;
		break;
	}

	delete[] errors;
	glDeleteShader(shader);
	return 0;
}

GLuint CreateShaderProgram(const std::string& vertex_source_path, const std::string& fragment_source_path) {
	// make an empty program
	GLuint program = glCreateProgram();
	// compile some shaders to attach to it
	GLuint vertex_shader = CompileShader(vertex_source_path, GL_VERTEX_SHADER);
	GLuint fragment_shader = CompileShader(fragment_source_path, GL_FRAGMENT_SHADER);

	// attach them
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	
	// link them and make sure they work
	glLinkProgram(program);
	glValidateProgram(program);

	// delete the compiled shaders now that everything is linked up.
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return program;
}

GLuint CreateComputeShaderProgram(const std::string& compute_source_path) {
	// make an empty program
	GLuint program = glCreateProgram();

	// compile a compute shader to attach to it
	GLuint compute_shader = CompileShader(compute_source_path, GL_COMPUTE_SHADER);

	// attach it
	glAttachShader(program, compute_shader);

	// link it and make sure it works
	glLinkProgram(program);
	glValidateProgram(program);

	// delete the compiled shader after the program is linked
	glDeleteShader(compute_shader);

	return program;
}