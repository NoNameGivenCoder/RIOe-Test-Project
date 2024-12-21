#version 330 core

// Inputs from the vertex shader
in vec3 FragPos;        // Position of the fragment in world space
in vec3 Normal;         // Normal in world space
in vec2 TexCoord;  // Color gradient based on vertex position

// Output to the framebuffer
out vec4 FragColor;

uniform sampler2D texture0;

void main()
{
    vec4 texColor = texture(texture0, TexCoord);

    // Ambient lighting (low intensity background light)
    float ambientStrength = 1;
    vec3 result = texColor.rgb;
    
    FragColor = vec4(result, texColor.a); // Final fragment color
}