#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D positionTexture;

void main()
{
    vec4 color = texture(positionTexture, TexCoords);
    FragColor = color;
} 