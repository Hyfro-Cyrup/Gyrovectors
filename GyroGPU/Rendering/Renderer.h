#pragma once
#include <vector>

#include <GL/glew.h>

#include "RenderPrimitive.h"
#include "Camera.h"

class Renderer {
private:
	GLuint quadVBO, quadVAO,
		compute_shader_program, 
		regular_shader_program, 
		texture, ssbo, 
		viewMatrixLoc, 
		projectionMatrixLoc;

	int _width, _height;

public:
	Renderer(int width, int height);

	void Render(Camera& worldCamera);

	void Resize(int width, int height);

	void PopulateComputeBuffer(const std::vector<RenderPrimitive>& primitives);


private:
	void GLEW_Setup();

	GLuint CreateTexture() const;

	void CreateQuad(GLuint& quadVBO, GLuint& quadVAO);

	void DispatchComputeShader() const;

	void RenderQuad() const;
};