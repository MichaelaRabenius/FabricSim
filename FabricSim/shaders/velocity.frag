#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D oldVelocityTexture;
uniform sampler2D accelerationTexture;
//uniform float timestep;

// texture size is 800x800, thus the offset to a neighboring texel should be 1/800?
//float texture_offset = 0.00125f;
uniform float texture_offset_x;
uniform float texture_offset_y; 
uniform float rest_dist;

float texture_offset = 0.05;

void main()
{
    //Provisional time step
    float dt = 0.002;

    //Same question, is our texture vec3 or vec4
    vec4 velocityData = texture(oldVelocityTexture, TexCoords);
    vec3 oldVelocity = velocityData.rgb;
    vec3 acceleration = texture(accelerationTexture, TexCoords).rgb;
    // integrate
    vec3 newVelocity = oldVelocity + acceleration * dt;

    FragColor = vec4(newVelocity, velocityData.w);

}
