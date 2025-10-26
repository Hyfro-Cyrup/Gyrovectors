#pragma once
#include <string>
#include <GL/glew.h>

class Texture
{
	GLuint id;
	int width, height;
	int unit{ 0 };

public:
	Texture(int unit, const std::string& path);
	~Texture();
	GLuint getId() const;

	int getWidth() const { return width; }
	int getHeight() const { return height; }
	int getUnit() const { return unit; }

    // Copy constructor
    Texture(const Texture& other);

    // Move constructor
	Texture(Texture&& other) noexcept;

    // Copy assignment operator
    Texture& operator=(const Texture& other);

    // Move assignment operator
	Texture& operator=(Texture&& other) noexcept;

};

