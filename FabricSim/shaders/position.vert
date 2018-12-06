#version 330 core
layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_TexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = in_TexCoords;
    gl_Position = vec4(in_Position, 1.0f); 
}  