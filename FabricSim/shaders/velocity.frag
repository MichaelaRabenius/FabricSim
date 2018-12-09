#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D oldVelocityTexture;
uniform sampler2D positionTexture;
//uniform float timestep;

float dist = 0.05f;

// texture size is 800x800, thus the offset to a neighboring texel should be 1/800?
//float texture_offset = 0.00125f;
float texture_offset = 0.05f; 

//Calculate the internal force by accumulating the forces of the neighboring particles
vec3 calculateInternalForces();

vec3 neighborForce(vec3 current_pos, vec3 current_speed, vec3 neighbor_pos, vec3 neighbor_speed);


void main()
{
    //Provisional time step
    float dt = 0.01;

    //Same question, is our texture vec3 or vec4
    vec3 oldVelocity = texture(oldVelocityTexture, TexCoords).rgb;
    vec3 pos = texture(positionTexture, TexCoords - vec2(texture_offset, 0.0f)).rgb;

    // In reality, the fabric is going to be affected by both internal and external forces.
    // Therefore we must calculate the acceleration from F = m*a
    // where F = F_internal + F_external(such as wind and gravity)
    
    //mass of particle
    float mass = 1.0f;

    // Internal force
    // This comes from the mass spring system (Hook's law (F = k * delta_x))
    // See section 12.3 in the course book

    //Pseudocode
    // vec3 dist = vec3(0.05f, 0.0f, 0.0f); // Current distance to neighbor particle
    // float rest_dist = 0.05f; // rest distance between particles
    // vec3 speed_diff = vec3(0.05f, 0.0f, 0.0f); //speed difference between neighboring particles, used for damping
    // float ks = 0.01f, kd = 0.01f; //constants


    vec3 internal_force = calculateInternalForces();

    //External forces
    vec3 wind = vec3(0.3f, 0.0f, 0.3f); // for testing only
    vec3 gravity = vec3(0.0f, -0.001f, 0.0f); //simple gravitational pull

    vec3 acceleration = ( internal_force + gravity) / mass;
    
    // integrate
    vec3 newVelocity = oldVelocity + gravity * dt;

    FragColor = vec4(newVelocity, 1.0);

}


//Calculate the internal force by accumulating the forces of the neighboring particles
vec3 calculateInternalForces() {

    //Current position and velocity
    vec3 current_pos = texture(positionTexture, TexCoords).rgb;
    vec3 current_speed = texture(oldVelocityTexture, TexCoords).rgb;

    //1. the 4-Neighborhood (i.e. the 4 closest neighbors)
    vec3 neighbor1_pos = texture(positionTexture, TexCoords + vec2(texture_offset, 0.0f)).rgb;
    vec3 neighbor2_pos = texture(positionTexture, TexCoords - vec2(texture_offset, 0.0f)).rgb;
    vec3 neighbor3_pos = texture(positionTexture, TexCoords + vec2(0.0f, texture_offset)).rgb;
    vec3 neighbor4_pos = texture(positionTexture, TexCoords - vec2(0.0f, texture_offset)).rgb;

    vec3 neighbor1_speed = texture(oldVelocityTexture, TexCoords + vec2(texture_offset, 0.0f)).rgb;
    vec3 neighbor2_speed = texture(oldVelocityTexture, TexCoords - vec2(texture_offset, 0.0f)).rgb;
    vec3 neighbor3_speed = texture(oldVelocityTexture, TexCoords + vec2(0.0f, texture_offset)).rgb;
    vec3 neighbor4_speed = texture(oldVelocityTexture, TexCoords - vec2(0.0f, texture_offset)).rgb;

    vec3 force1 = neighborForce(current_pos, current_speed, neighbor1_pos, neighbor1_speed);
    force1 += neighborForce(current_pos, current_speed, neighbor2_pos, neighbor3_speed);
    force1 += neighborForce(current_pos, current_speed, neighbor3_pos, neighbor3_speed);
    force1 += neighborForce(current_pos, current_speed, neighbor4_pos, neighbor4_speed);

    //2. the 8-Neighborhood (i.e. the 8 closest neighbors)
    vec3 neighbor5_pos = texture(positionTexture, TexCoords + vec2(texture_offset, texture_offset)).rgb;
    vec3 neighbor6_pos = texture(positionTexture, TexCoords + vec2(texture_offset, -texture_offset)).rgb;
    vec3 neighbor7_pos = texture(positionTexture, TexCoords + vec2(-texture_offset, texture_offset)).rgb;
    vec3 neighbor8_pos = texture(positionTexture, TexCoords + vec2(-texture_offset, -texture_offset)).rgb;

    vec3 neighbor5_speed = texture(oldVelocityTexture, TexCoords + vec2(texture_offset, texture_offset)).rgb;
    vec3 neighbor6_speed = texture(oldVelocityTexture, TexCoords + vec2(texture_offset, -texture_offset)).rgb;
    vec3 neighbor7_speed = texture(oldVelocityTexture, TexCoords + vec2(-texture_offset, texture_offset)).rgb;
    vec3 neighbor8_speed = texture(oldVelocityTexture, TexCoords + vec2(-texture_offset, -texture_offset)).rgb;

    vec3 force2 = neighborForce(current_pos, current_speed, neighbor5_pos, neighbor5_speed);
    force2 += neighborForce(current_pos, current_speed, neighbor6_pos, neighbor6_speed);
    force2 += neighborForce(current_pos, current_speed, neighbor7_pos, neighbor7_speed);
    force2 += neighborForce(current_pos, current_speed, neighbor8_pos, neighbor8_speed);
    
    //3. the 12-Neighborhood (i.e. the 12 closest neighbors)
    float double_offset = texture_offset * 2.0f;
    vec3 neighbor9_pos = texture(positionTexture, TexCoords + vec2(double_offset, 0.0f)).rgb;
    vec3 neighbor10_pos = texture(positionTexture, TexCoords + vec2(-double_offset, 0.0f)).rgb;
    vec3 neighbor11_pos = texture(positionTexture, TexCoords + vec2(0.0f, double_offset)).rgb;
    vec3 neighbor12_pos = texture(positionTexture, TexCoords + vec2(0.0f, -double_offset)).rgb;

    vec3 neighbor9_speed = texture(oldVelocityTexture, TexCoords + vec2(double_offset, 0.0f)).rgb;
    vec3 neighbor10_speed = texture(oldVelocityTexture, TexCoords + vec2(-double_offset, 0.0f)).rgb;
    vec3 neighbor11_speed = texture(oldVelocityTexture, TexCoords + vec2(0.0f, double_offset)).rgb;
    vec3 neighbor12_speed = texture(oldVelocityTexture, TexCoords + vec2(0.0f, -double_offset)).rgb;

    vec3 force3 = neighborForce(current_pos, current_speed, neighbor9_pos, neighbor9_speed);
    force3 += neighborForce(current_pos, current_speed, neighbor10_pos, neighbor10_speed);
    force3 += neighborForce(current_pos, current_speed, neighbor11_pos, neighbor11_speed);
    force3 += neighborForce(current_pos, current_speed, neighbor12_pos, neighbor12_speed);

    vec3 result_force = force1 + force2 + force3;
    return result_force;
}

vec3 neighborForce(vec3 current_pos, vec3 current_speed, vec3 neighbor_pos, vec3 neighbor_speed) {

    float rest_dist = 0.05f;
    //Calculate distance to the neighbor
    vec3 dist = neighbor_pos - current_pos;

    vec3 speed_diff = neighbor_speed - current_speed; //speed difference between neighboring particles, used for damping
    
    float ks = 0.01f, kd = 0.01f; //constants


    vec3 internal_force = -(ks * (length(dist) - rest_dist) + (kd * (dot(dist, speed_diff) / length(dist)))) * (dist / length(dist));
    return internal_force;
}
