#include "Texture.h"

#include <iostream>
#include <string>

#include <GL/glew.h>
#include <stb_image.h>

Texture::Texture(int unit, const std::string& path) : unit{ unit } {
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// load and generate the texture
	int nrChannels;
	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture from " << path << std::endl;
	}
	stbi_image_free(data);
	glBindTextureUnit(unit, id);
}

Texture::~Texture() {
	glDeleteTextures(1, &id);
}

GLuint Texture::getId() const { return id; }


// Copy constructor
Texture::Texture(const Texture& other) : id{ other.id }, width{ other.width }, height{ other.height } {}

Texture::Texture(Texture&& other) noexcept : id{ other.id }, width{ other.width }, height{ other.height } {
	other.id = 0;
}

// Copy assignment operator
Texture& Texture::operator=(const Texture& other) {
	if (this != &other) {
		id = other.id;
		width = other.width;
		height = other.height;
	}
	return *this;
}

// Move assignment operator
Texture& Texture::operator=(Texture&& other) noexcept {
	if (this != &other) {
		id = other.id;
		width = other.width;
		height = other.height;
		other.id = 0;
	}
	return *this;
}
