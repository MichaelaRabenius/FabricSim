#version 330 core
layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_TexCoords;

out vec2 TexCoords;

//This shader is the only one that is going to act on the fabric
// The rest will only act on the screen quad, meaning that is shader
//is the only one that needs 3 inputs.
uniform sampler2D positionTexture;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// I should modelview and projection matrices to this shader...

void main()
{   
    vec3 pos = texture(positionTexture, in_TexCoords).rgb;
    TexCoords = in_TexCoords;
    //gl_Position = projection * view * model * vec4(pos, 1.0f); // THIS IS WHAT SHALL HAPPEN IN THE FUTURE
    gl_Position = projection * view * model * vec4(in_Position, 1.0f); // FOR NOW, WE SIMPLY TEST THE TEXTURE IN THE FRAGMENT SHADER
}  