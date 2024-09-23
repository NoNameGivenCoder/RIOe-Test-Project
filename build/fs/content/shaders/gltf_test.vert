#version 330 core

// Input attributes from the vertex buffer
layout(location = 0) in vec3 aPos;        // Vertex position (location = 0)
layout(location = 1) in vec3 aNormal;     // Vertex normal (location = 1)
layout(location = 2) in vec2 aTexCoord;   // Vertex texture coordinates (location = 2)

// Uniforms for transformation matrices
uniform mat4 model;          // Model matrix
uniform mat4 view;           // View matrix
uniform mat4 projection;

// Output variables to pass to the fragment shader
out vec3 FragPos;         // Position of the fragment in world space
out vec3 Normal;          // Normal in world space
out vec3 GradientColor;    // Color for the gradient based on position

void main()
{
    // Transform the normal to world space
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // Transform the vertex position to world space
    FragPos = vec3(model * vec4(aPos, 1.0));

    GradientColor = vec3(aPos.y, aPos.x, aPos.z) * 2;

    // Calculate the final position
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}