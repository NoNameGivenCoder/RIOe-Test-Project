#version 330 core

// Inputs from the vertex shader
in vec3 FragPos;        // Position of the fragment in world space
in vec3 Normal;         // Normal in world space
in vec3 GradientColor;  // Color gradient based on vertex position

// Output to the framebuffer
out vec4 FragColor;

uniform vec3 viewPos;       // Camera/view position in world space

void main()
{
    // Ambient lighting (low intensity background light)
    float ambientStrength = 1;
    vec3 ambient = ambientStrength * GradientColor;
    // Combine the lighting components (ambient + diffuse + specular)
    vec3 result = ambient;
    
    FragColor = vec4(result, 1.0); // Final fragment color
}