#version 330 core

uniform mat4 proj_view_mtx;
uniform mat4 model_mtx;

layout(location = 0) in vec4 a_color;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec4 a_position;
layout(location = 3) in vec3 a_tangent;
layout(location = 4) in vec2 a_texCoord;

out vec4 color;
out vec2 texCoord;
out vec3 FragPos;
out vec3 Normal;

void main()
{
    FragPos = vec3(model_mtx * a_position);
    Normal = mat3(transpose(inverse(model_mtx))) * a_normal;
    color = a_color;
    texCoord = a_texCoord;
    gl_Position = proj_view_mtx * model_mtx * a_position;
}