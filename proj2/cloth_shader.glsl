#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture;

void main()
{
    vec4 color = texture(texture, TexCoord);
    color.a = 0.5; // Set the alpha value to make it transparent
    FragColor = color;
}