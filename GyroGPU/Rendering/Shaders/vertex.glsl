#version 430 core

layout(location = 0) in vec2 inPosition;  // Input vertex position
layout(location = 1) in vec2 inTexCoords; // Input texture coordinates

uniform mat4 view;
uniform mat4 projection;

out vec2 fragTexCoords; // Output texture coordinates

void main() {
    fragTexCoords = inTexCoords;
    gl_Position = projection * view * vec4(inPosition, 0.0, 1.0); 
}