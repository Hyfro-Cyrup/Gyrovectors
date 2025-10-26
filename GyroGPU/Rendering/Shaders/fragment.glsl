#version 430 core

in vec2 fragTexCoords;  // Input texture coordinates from the vertex shader
out vec4 fragColor;     // Output color to the screen

uniform sampler2D outputTexture; // The texture that the compute shader wrote to

void main() {
    fragColor = texture(outputTexture, fragTexCoords);  // Sample the texture
}
