#version 330 core

uniform mat4 proj_view_mtx;
uniform mat4 model_mtx;

layout(location = 0) in vec4 aColor;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec4 aPosition;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec2 aTexCoord;

out vec4 color;
out vec2 texCoord;

void main()
{
    color = aColor;
    texCoord = aTexCoord;
    gl_Position = proj_view_mtx * model_mtx * aPosition;
}