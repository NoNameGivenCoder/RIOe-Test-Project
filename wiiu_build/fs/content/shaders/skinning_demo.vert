#version 330 core

layout(std140) uniform SkeletalBlock {
    mat4 viewProj;          // View-projection matrix
    mat4 modelMtx;
};

layout(std140) uniform SkeletalBoneBlock {
    mat4 bones[40];       // Array of bone transforms
};

layout(location = 0) in vec3 aPos;         // Vertex position
layout(location = 1) in vec3 aNormal;      // Vertex normal
layout(location = 2) in vec2 aTexCoord;    // Vertex texture coordinates
layout(location = 3) in vec4 aBoneWeights; // Weights of the 4 bones influencing the vertex
layout(location = 4) in uvec4 aBoneIndices;// Indices of the 4 bones influencing the vertex

out vec2 TexCoord;         // Output: Texture coordinates
out vec3 FragNormal;       // Output: World-space normal
out vec4 Color;            // Output: Color (optional, used for debugging or other purposes)
out vec3 FragPos;

void main()
{
    TexCoord = aTexCoord;

    // Skinning matrix based on bone weights and bone indices
    mat4 skinMatrix =
        aBoneWeights.x * bones[int(aBoneIndices.x)] +
        aBoneWeights.y * bones[int(aBoneIndices.y)] +
        aBoneWeights.z * bones[int(aBoneIndices.z)] +
        aBoneWeights.w * bones[int(aBoneIndices.w)];

    vec4 worldPos = skinMatrix * vec4(aPos, 1.0f);
    FragPos = vec3(worldPos);
    FragNormal = mat3(skinMatrix) * aNormal;

    // Transform vertex position by the skinning matrix and then by the view-projection matrix
    gl_Position = viewProj * worldPos;
}
