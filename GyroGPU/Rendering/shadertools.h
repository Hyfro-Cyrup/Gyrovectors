#pragma once
#include <string>

#include <GL/glew.h>

GLuint CompileShader(std::string shader_source_path, GLuint type);

GLuint CreateShaderProgram(const std::string& vertex_source_path, const std::string& fragment_source_path);

GLuint CreateComputeShaderProgram(const std::string& compute_source_path);