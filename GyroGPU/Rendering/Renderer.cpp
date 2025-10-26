#include "Renderer.h"
#include <algorithm>
#include <stdexcept>
#include <vector>

#include <GL/glew.h>

#include "RenderPrimitive.h"
#include "shadertools.h"
#include <iostream>

Renderer::Renderer(int width, int height) : _width{ width }, _height{ height } {
	GLEW_Setup(); // must come after the window and context

	// define the opengl viewport
	glViewport(0, 0, _width, _height);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	CreateQuad(quadVBO, quadVAO);


	compute_shader_program = CreateComputeShaderProgram("Rendering/Shaders/compute.glsl");
	regular_shader_program = CreateShaderProgram("Rendering/Shaders/vertex.glsl", "Rendering/Shaders/fragment.glsl");
	texture = CreateTexture();

	glUseProgram(compute_shader_program);
	glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	GLint loc = glGetUniformLocation(compute_shader_program, "textures");
	int units[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ,11, 12, 13, 14, 15 };
	glUniform1iv(loc, 16, units);

	loc = glGetUniformLocation(compute_shader_program, "WIDTH");
	glUniform1i(loc, static_cast<GLint>(_width));

	loc = glGetUniformLocation(compute_shader_program, "HEIGHT");
	glUniform1i(loc, static_cast<GLint>(_height));


	// establish the background color (I think this only has to happen once)
	glClearColor(0.2f, 0.8f, 0.4f, 1.0f);
}

void Renderer::Render() {
	// clear the screen
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DispatchComputeShader();

	RenderQuad();

	glUseProgram(0);
}

void Renderer::Resize(int width, int height) {
	_width = width;
	_height = height;
	glViewport(0, 0, _width, _height);
	glDeleteTextures(1, &texture);
	texture = CreateTexture();

	glUseProgram(compute_shader_program);
	glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	GLuint loc = glGetUniformLocation(compute_shader_program, "WIDTH");
	glUniform1i(loc, static_cast<GLint>(_width));

	loc = glGetUniformLocation(compute_shader_program, "HEIGHT");
	glUniform1i(loc, static_cast<GLint>(_height));
}

void Renderer::PopulateComputeBuffer(const std::vector<RenderPrimitive>& primitives) {
	static size_t allocatedCount = 0;
	constexpr size_t MAX_COUNT = 512; // starting point, not actual max

	size_t neededCount = primitives.size();
	size_t targetCount = std::max(neededCount, MAX_COUNT);

	// First-time allocation or resize if needed
	if (ssbo == 0 || neededCount > allocatedCount) {
		allocatedCount = targetCount;
		if (ssbo == 0)
			glGenBuffers(1, &ssbo);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER,
			allocatedCount * sizeof(RenderPrimitive),
			nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
	}

	// update the data in the buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
		neededCount * sizeof(RenderPrimitive), primitives.data());

	// Update primitiveCount uniform
	glUseProgram(compute_shader_program);
	GLint loc = glGetUniformLocation(compute_shader_program, "primitiveCount");
	glUniform1i(loc, static_cast<GLint>(neededCount));
}

void Renderer::GLEW_Setup() {
	// don't fear the 'experimental'; this just enables shit.
	glewExperimental = GL_TRUE;

	if (GLEW_OK != glewInit()) {
		throw std::runtime_error("Failed to iniitialize GLEW");
	}
}

GLuint Renderer::CreateTexture() const {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Initialize texture with empty data (RGBA, 8-bit per channel)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _width, _height, 0, GL_RGBA, GL_FLOAT, nullptr);

	// Set texture parameters for wrapping and filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Unbind the texture
	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

void Renderer::CreateQuad(GLuint& quadVBO, GLuint& quadVAO) {
	GLfloat quadVertices[] = {
		// Positions    // Texture Coordinates
		-1.0f,  1.0f,    0.0f, 1.0f,  // Top-left
		-1.0f, -1.0f,    0.0f, 0.0f,  // Bottom-left
		 1.0f,  1.0f,    1.0f, 1.0f,  // Top-right

		-1.0f, -1.0f,    0.0f, 0.0f,  // Bottom-left
		 1.0f, -1.0f,    1.0f, 0.0f,  // Bottom-right
		 1.0f,  1.0f,    1.0f, 1.0f   // Top-right
	};
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);

	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Texture coordinates attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);  // Unbind the VAO
}

void Renderer::DispatchComputeShader() const {
	glUseProgram(compute_shader_program);
	// Dispatch the compute shader with workgroups. 
	// The workgroup size is 16x16, so we need to calculate the number of workgroups in each dimension.
	int workgroup_size_x = (_width + 15) / 16;
	int workgroup_size_y = (_height + 15) / 16;

	glDispatchCompute(workgroup_size_x, workgroup_size_y, 1);

	// Wait for compute shader to finish
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Renderer::RenderQuad() const {
	glBindTexture(GL_TEXTURE_2D, texture);
	glUseProgram(regular_shader_program);  // Use the shader program for rendering

	glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
	glBindTexture(GL_TEXTURE_2D, texture); // Bind the texture you want to render

	// Set the texture uniform in the fragment shader
	glUniform1i(glGetUniformLocation(regular_shader_program, "outputTexture"), 0); // Texture unit 0

	// Render the full-screen quad
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);  // 6 vertices for the full-screen quad
	glBindVertexArray(0);  // Unbind the VAO
}

