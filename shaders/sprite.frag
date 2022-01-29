#version 330 core

in vec2 TexCoords;
in vec3 Pos;

out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;
uniform int Lights;
uniform vec3 LightPos;

void main()
{
    color = vec4(spriteColor, 1.0) * texture(image, TexCoords);

    if (Lights == 0)
    {
        float distance = length(Pos - LightPos);
        if (distance < 100)
        {
            color = vec4(vec3(1.0f)*(1/distance)*(1/distance)*vec3(color) * 1000,color.w);
            // float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
            // color = vec4(attenuation * vec3(color) * 10,color.w);
            
        }
        else
        {
            color = vec4(vec3(0.0f),1.0);
        }
    }

}