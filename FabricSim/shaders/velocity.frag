#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D oldVelocityTexture;
uniform sampler2D accelerationTexture;

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
