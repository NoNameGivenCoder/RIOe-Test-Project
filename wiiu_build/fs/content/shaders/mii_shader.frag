#version 330 core

uniform int u_mode;
uniform vec3 u_const1;
uniform vec3 u_const2;
uniform vec3 u_const3;
uniform vec3 u_SunPosition;

struct PS_PUSH_DATA
{
    uint alphaFunc;
    float alphaRef;
};

uniform PS_PUSH_DATA PS_PUSH;

uniform sampler2D s_texture;

in vec4 color;
in vec2 texCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

void main(void)
{
    // I honestly don't know what to do with the vertex color

    // No Texture
    if (u_mode == 0)
    {
	float downFactor = dot(Normal, vec3(0.0, -1.0, 0.0));
        if (downFactor > 0.15)
    	{
        	FragColor = vec4(u_const1 * 0.7, 0); // Darken the color a bit
    	}
    	else
    	{
        	FragColor = vec4(u_const1, 0);
    	}
    }
    // Assuming mask
    else if (u_mode == 1)
        FragColor = texture(s_texture, texCoord);
    // Has Texture
    else if (u_mode == 2)
    {
        vec4 textureColor = texture(s_texture, texCoord);
        FragColor = vec4(
            u_const1 * textureColor.r +
            u_const2 * textureColor.g +
            u_const3 * textureColor.b,
            textureColor.a
        );
    }
    else if (u_mode == 3)
    {
        vec4 textureColor = texture(s_texture, texCoord);
        FragColor = vec4(
            u_const1 * textureColor.r,
            textureColor.r
        );
    }
    else if (u_mode == 4)
    {
        vec4 textureColor = texture(s_texture, texCoord);
        FragColor = vec4(
            u_const1 * textureColor.g,
            textureColor.r
        );
    }
    else if (u_mode == 5)
    {
        vec4 textureColor = texture(s_texture, texCoord);
        FragColor = vec4(
            u_const1 * textureColor.r,
            1.0
        );
    }
}