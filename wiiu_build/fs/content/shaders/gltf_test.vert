#version 330 core

uniform mat4 parentNodeMtx;
uniform mat4 normalMtx;
uniform mat4 viewProj;

// Input attributes from the vertex buffer
layout(location = 0) in vec3 aPos;        // Vertex position (location = 0)
layout(location = 1) in vec3 aNormal;     // Vertex normal (location = 1)
layout(location = 2) in vec2 aTexCoord;   // Vertex texture coordinates (location = 2)

// Output variables to pass to the fragment shader
out vec3 FragPos;         // Position of the fragment in world space
out vec3 Normal;          // Normal in world space
out vec2 TexCoord;    // Color for the gradient based on position

void main()
{
    // Set the texture coordinates for the fragment shader
    TexCoord = aTexCoord;

    // Calculate the world-space position of the vertex
    vec4 worldPos = parentNodeMtx * vec4(aPos, 1.0);

    // Calculate the final clip-space position for rendering
    gl_Position = viewProj * worldPos;
}