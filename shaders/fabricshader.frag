#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 exSurface;

uniform sampler2D stripedTexture;


void main()
{   
    vec4 color = texture(stripedTexture, TexCoords);

    vec4 icolor = vec4(1 - color.r, 1 - color.g, 1 - color.b, 1.0);
    FragColor = vec4(0.5, 0.1, 0.3, 1.0);
    FragColor = color;
} 