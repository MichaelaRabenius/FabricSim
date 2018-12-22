#version 330 core
layout (location = 0) in vec2 in_Position;
layout (location = 1) in vec2 in_TexCoords;

//This shader will only act on a 2D quad, much like the plaintexture shader.
//The vertex shader will not do anything, leaving the computation to the 
//fragment shader.

out vec2 TexCoords;

void main()
{
    TexCoords = in_TexCoords;
    gl_Position = vec4(in_Position.x, in_Position.y, 0.0, 1.0); 
}  