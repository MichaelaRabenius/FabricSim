#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float u_timer;

void main()
{	
	float amt = 2.5 * sin(aPos.y * u_timer) + 1.0;
	vec3 dis = aPos;
	vec3 bPos = aPos + amt * dis;
    gl_Position = projection * view * model * vec4(bPos, 1.0);
	//gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    ourColor = aColor;
}