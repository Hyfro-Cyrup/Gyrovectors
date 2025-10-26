#version 430 core

layout(location = 0) in vec2 inPosition;  // Input vertex position
layout(location = 1) in vec2 inTexCoords; // Input texture coordinates

out vec2 fragTexCoords; // Output texture coordinates

void main() {
    fragTexCoords = inTexCoords;    // Pass texture coordinates to fragment shader
    gl_Position = vec4(inPosition, 0.0, 1.0); // Pass position to NDC space
}