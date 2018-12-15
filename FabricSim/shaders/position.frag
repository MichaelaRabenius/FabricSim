#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D oldpositionTexture;
uniform sampler2D velocityTexture;
uniform sampler2D accelerationTexture;
uniform sampler2D positionTexture;

uniform float timestep;

void main()
{
	//Provisional time step
    float dt = 0.002;

	//Pseudocode
    vec3 oldPos = texture(oldpositionTexture, TexCoords).rgb; //Note to self: will the texture contain only rgb, not rgba?
    vec3 velocity = texture(velocityTexture, TexCoords).rgb;
	vec4 acceleration = texture(accelerationTexture, TexCoords);
	vec3 currentPos = texture(positionTexture, TexCoords).rgb;
	
	//vec3 newPos = oldPos + velocity * dt;
	vec3 newPos = 2.0 * currentPos -  oldPos + acceleration.rgb * dt * dt;

	// if(currentPos.w == 1.0) {
	// 	newPos = vec3(0.0, 0.0, 1.0);
	// }
	
	FragColor = vec4(newPos, 1.0);
	//FragColor = vec4(1.0, 1.0, 0.0, 1.0);
} 