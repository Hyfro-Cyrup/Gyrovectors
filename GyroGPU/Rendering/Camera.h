#pragma once

#include <GL/glew.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>

class Camera
{
	glm::vec3 eye, center, up;
	float aspectRatio;
	float scale;

public:
	Camera(int, int);

	glm::mat4 getProjectionMatrix() const;
	glm::mat4 getViewMatrix() const;

	void bindUniforms(GLuint, GLuint);

	// TODO: implement cool matrix math stuff to make this motion relative to local coordinates instead of world coordinates
	void move(GLfloat, GLfloat, GLfloat);
	void point(GLfloat, GLfloat, GLfloat);
	void strafe(GLfloat, GLfloat);

	void setAspectRatio(double ratio);
	void setAspectRatio(int width, int height);

	void zoom(int direction);

	void lookAt(glm::vec3 eye, glm::vec3 center, GLfloat scale);
};



