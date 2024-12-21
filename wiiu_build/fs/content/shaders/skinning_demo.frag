#version 330 core

in vec2 TexCoord;
in vec3 FragNormal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D texture0;

layout(std140) uniform EnvironmentBlock {
    vec3 light_color;          // View-projection matrix
    vec3 lightPos;
};

void main()
{
    // Normalize inputs
    vec3 norm = normalize(FragNormal);       // Surface normal
    vec3 lightDir = normalize(lightPos - FragPos);  // Direction from fragment to light

    // Ambient lighting (constant low-level light)
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * light_color;

    // Diffuse lighting (light scattered by surface)
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light_color;

    float specularStrength = 0;
    vec3 viewDir = normalize(-FragPos);      // Direction to the camera (assuming camera at origin)
    vec3 reflectDir = reflect(-lightDir, norm); // Reflect light direction around normal
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);  // Shininess factor
    vec3 specular = specularStrength * spec * light_color;

    // Combine results
    vec3 lighting = (ambient + diffuse + specular);

    // Sample texture color
    vec4 texColor = texture(texture0, TexCoord);

    // Final output color
    FragColor = vec4(lighting * texColor.rgb, texColor.a);
}