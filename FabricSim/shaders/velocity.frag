#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D oldVelocityTexture;
//uniform float timestep;

void main()
{
    //Provisional time step
    float dt = 0.03;

    //Same question, is our texture vec3 or vec4
    vec3 oldVelocity = texture(oldVelocityTexture, TexCoords).rgb;
    
    // We're gonna do a test first with a simple acceleration
    //float mass = 1.0f;
    float gravity = 9.82;
    vec3 acceleration = vec3(0.0f, -1.0f, 0.0f) * gravity; //simple gravitational pull
    
    vec3 newVelocity = oldVelocity + acceleration * dt;

    FragColor = vec4(newVelocity, 1.0);
} 