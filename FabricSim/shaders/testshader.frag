#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

void main()
{
    vec3 col = vec3(0.0f, 1.0f, 0.0f);
    FragColor = vec4(col, 1.0);
} 