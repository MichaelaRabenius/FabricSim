#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D positionTexture;

void main()
{
    vec3 color = texture(positionTexture, TexCoords).rgb;
    vec3 col = vec3(0.0f, 1.0f, 0.0f);
    FragColor = vec4(color, 1.0);
} 