#include "Camera.h"

#include <GL/glew.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera::Camera(int width, int height) :
	eye{ 0.0f, 0.0f, 1.0f },
	center{ 0.0f, 0.0f,  0.0f },
	up{ 0.0f, 1.0f, 0.0f },
	aspectRatio{ (float)width / (float)height },
	scale{ 1.0f } {
}

glm::mat4 Camera::getProjectionMatrix() const {
	return glm::orthoNO(-aspectRatio * scale, aspectRatio * scale, -1.0f * scale, 1.0f * scale, 0.0f, 10.0f);
}

glm::mat4 Camera::getViewMatrix() const {
	return glm::lookAt(eye, center, up);
}

void Camera::bindUniforms(GLuint viewMatrixLoc, GLuint projectionMatrixLoc) {
	glm::mat4 projection = getProjectionMatrix();
	glm::mat4 lookat = getViewMatrix();

	glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(lookat));
	glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void Camera::move(GLfloat dx, GLfloat dy, GLfloat dz) {
	eye += glm::vec3(dx, dy, dz) * scale;
}

void Camera::point(GLfloat dx, GLfloat dy, GLfloat dz) {
	center += glm::vec3(dx, dy, dz) * scale;
}

void Camera::setAspectRatio(double ratio) { aspectRatio = (float)ratio; }
void Camera::setAspectRatio(int width, int height) { aspectRatio = (float)width / (float)height; }

void Camera::zoom(int direction) {
	float factor = (direction == 1 ? 0.5f : 2.0f);
	scale *= factor;
}

/// <summary>
/// Set the camera position and orientation explicitly.
/// </summary>
/// <param name="new_eye">The new location of the camera.</param>
/// <param name="new_center">The new point at which the camera is looking.</param>
/// <param name="new_scale">The new height of the screen, in world coordinates.</param>
void Camera::lookAt(glm::vec3 new_eye, glm::vec3 new_center, GLfloat new_scale)
{
	eye = new_eye;
	center = new_center;
	scale = new_scale;
}