#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

void main()
{
    vec4 color = vec4(0.4, 0.2, 0.6, 1.0);
    FragColor = color;
} 