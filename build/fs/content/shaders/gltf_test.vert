#version 330 core

layout(std140)
uniform cViewBlock
{
    vec3 viewPos;
    vec4 viewProj[4];
};

layout(std140)
uniform cModelBlock
{
    vec4 model[3];
    vec4 normalMtx[3];
};

// Input attributes from the vertex buffer
layout(location = 0) in vec3 aPos;        // Vertex position (location = 0)
layout(location = 1) in vec3 aNormal;     // Vertex normal (location = 1)
layout(location = 2) in vec2 aTexCoord;   // Vertex texture coordinates (location = 2)

// Output variables to pass to the fragment shader
out vec3 FragPos;         // Position of the fragment in world space
out vec3 Normal;          // Normal in world space
out vec3 GradientColor;    // Color for the gradient based on position

void main()
{
    vec4 normal = vec4(aNormal, 0.0);

    Normal = normalize(vec3(
        dot(normalMtx[0], normal),
        dot(normalMtx[1], normal),
        dot(normalMtx[2], normal)
    ));

    vec4 pos = vec4(aPos, 1.0f);

    FragPos = vec3(
        dot(model[0], pos),
        dot(model[1], pos),
        dot(model[2], pos)
    );
    vec4 wpos = vec4(FragPos, 1.0);

    gl_Position = vec4(
        dot(viewProj[0], wpos),
        dot(viewProj[1], wpos),
        dot(viewProj[2], wpos),
        dot(viewProj[3], wpos)
    );
}