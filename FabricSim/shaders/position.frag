#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D oldpositionTexture;
uniform sampler2D velocityTexture;
uniform sampler2D accelerationTexture;

uniform float timestep;

void main()
{
	//Provisional time step
    float dt = 0.03;

	//Pseudocode
    vec3 oldPos = texture(oldpositionTexture, TexCoords).rgb; //Note to self: will the texture contain only rgb, not rgba?
    vec3 velocity = texture(velocityTexture, TexCoords).rgb;
	vec4 acceleration = texture(accelerationTexture, TexCoords);
	
	vec3 newPos = oldPos + velocity * dt;

	// if(acceleration.w == 1.0) {
	// 	newPos = vec3(0.0, 0.0, 1.0);
	// }
	
	FragColor = vec4(newPos, 1.0);
	//FragColor = vec4(1.0, 1.0, 0.0, 1.0);
} 