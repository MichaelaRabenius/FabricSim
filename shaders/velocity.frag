#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D oldVelocityTexture;
uniform sampler2D positionTexture;
//uniform float timestep;

// texture size is 800x800, thus the offset to a neighboring texel should be 1/800?
//float texture_offset = 0.00125f;
uniform float texture_offset_x;
uniform float texture_offset_y; 
uniform float rest_dist;
uniform float rest_dist2;
uniform float rest_dist3;


float texture_offset = 0.05;

//Calculate the internal force by accumulating the forces of the neighboring particles
vec3 calculateInternalForces();

vec3 neighborForce(vec3 current_pos, vec3 current_speed, vec3 neighbor_pos, vec3 neighbor_speed, float r);


void main()
{
    //Provisional time step
    float dt = 0.01;

    //Same question, is our texture vec3 or vec4
    vec4 velocityData = texture(oldVelocityTexture, TexCoords);
    vec3 oldVelocity = velocityData.rgb;
    vec3 pos = texture(positionTexture, TexCoords - vec2(texture_offset, 0.0f)).rgb;

    // In reality, the fabric is going to be affected by both internal and external forces.
    // Therefore we must calculate the acceleration from F = m*a
    // where F = F_internal + F_external(such as wind and gravity)
    
    //mass of particle
    float mass = 1.0f;

    vec3 internal_force = calculateInternalForces();

    //External forces
    vec3 wind = vec3(0.03f, 0.0f, 0.3f); // for testing only
    vec3 gravity = vec3(0.0f, -0.1f, 0.0f); //simple gravitational pull

    vec3 acceleration = (gravity + internal_force) / mass;
    
    if(velocityData.w == 1.0) {
        acceleration = vec3(0.0f, 0.0f, 0.0f);
    }

    // integrate
    vec3 newVelocity = oldVelocity + acceleration * dt;

    FragColor = vec4(newVelocity, velocityData.w);

}

vec3 getForceFromTexture(vec3 accumulated_force, vec3 current_pos, vec3 current_speed, vec2 texcoords, float r){
     
    if(texcoords.x >= 0 && texcoords.x <= 1 && texcoords.y >= 0 && texcoords.y <= 1) {
        vec3 neighbor_pos = texture(positionTexture, texcoords).rgb;
        vec3 neighbor_speed = texture(oldVelocityTexture, texcoords).rgb;
        accumulated_force += neighborForce(current_pos, current_speed, neighbor_pos, neighbor_speed, r);

    }
    
    return accumulated_force;
}


//Calculate the internal force by accumulating the forces of the neighboring particles
vec3 calculateInternalForces() {

    //Current position and velocity
    vec3 current_pos = texture(positionTexture, TexCoords).rgb;
    vec3 current_speed = texture(oldVelocityTexture, TexCoords).rgb;

    //compute neighbouring texture coordinates
    vec2 right = TexCoords + vec2(texture_offset_x, 0.0f);
    vec2 left = TexCoords - vec2(texture_offset_x, 0.0f);
    vec2 up = TexCoords + vec2(0.0f, texture_offset_y);
    vec2 down = TexCoords - vec2(0.0f, texture_offset_y);

    //1. the 4-Neighborhood (i.e. the 4 closest neighbors)
    vec3 force1 = vec3(0.0, 0.0, 0.0);

    force1 = getForceFromTexture(force1, current_pos, current_speed, right, rest_dist);
    force1 = getForceFromTexture(force1, current_pos, current_speed, left, rest_dist);
    force1 = getForceFromTexture(force1, current_pos, current_speed, up, rest_dist);
    force1 = getForceFromTexture(force1, current_pos, current_speed, down, rest_dist);


    //2. the 8-Neighborhood (i.e. the 8 closest neighbors)
    //compute neighbouring texture coordinates
    vec2 up_right = TexCoords + vec2(texture_offset_x, texture_offset_y);
    vec2 down_left = TexCoords + vec2(-texture_offset_x, -texture_offset_y);
    vec2 up_left = TexCoords + vec2(-texture_offset_x, texture_offset_y);
    vec2 down_right = TexCoords + vec2(texture_offset_x, -texture_offset_y);

    vec3 force2 = vec3(0.0, 0.0, 0.0);

    force2 = getForceFromTexture(force2, current_pos, current_speed, up_right, rest_dist2);
    force2 = getForceFromTexture(force2, current_pos, current_speed, up_left, rest_dist2);
    force2 = getForceFromTexture(force2, current_pos, current_speed, down_right, rest_dist2);
    force2 = getForceFromTexture(force2, current_pos, current_speed, down_left, rest_dist2);
    

    //3. the 12-Neighborhood (i.e. the 12 closest neighbors)
    vec3 force3 = vec3(0.0, 0.0, 0.0);
    
    vec2 right2 = TexCoords + vec2(2 * texture_offset_x, 0.0f);
    vec2 left2 = TexCoords - vec2(2 * texture_offset_x, 0.0f);
    vec2 up2 = TexCoords + vec2(0.0f, 2 * texture_offset_y);
    vec2 down2 = TexCoords - vec2(0.0f, 2 * texture_offset_y);

    force3 = getForceFromTexture(force3, current_pos, current_speed, right2, rest_dist3);
    force3 = getForceFromTexture(force3, current_pos, current_speed, left2, rest_dist3);
    force3 = getForceFromTexture(force3, current_pos, current_speed, up2, rest_dist3);
    force3 = getForceFromTexture(force3, current_pos, current_speed, down2, rest_dist3);

    vec3 result_force = force1 + force2 + force3;
    return result_force;
}

vec3 neighborForce(vec3 current_pos, vec3 current_speed, vec3 neighbor_pos, vec3 neighbor_speed, float r) {
    //Calculate distance to the neighbor
    vec3 dist = current_pos - neighbor_pos;

    vec3 speed_diff = current_speed - neighbor_speed; //speed difference between neighboring particles, used for damping
    
    float ks = 10.0, kd = 0.01f; //constants


    vec3 internal_force = -(ks * (length(dist) - r) + (kd * (dot(dist, speed_diff) / length(dist)))) * (dist / length(dist));
    return internal_force;
}
