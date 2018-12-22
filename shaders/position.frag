#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D positionTexture;
uniform sampler2D oldpositionTexture;

uniform float texture_offset_x;
uniform float texture_offset_y; 
uniform vec3 rest_distances;

uniform float radius = 0.2;
uniform vec3 center = vec3(0.0, -1.0, 0.0);

uniform float dt = 0.01;
uniform float damping = -0.0125f; //default damping as per suggestion

uniform bool hideSphere;

/*** FUNCTION DECLARATIONS ***/
//Calculate the internal force by accumulating the forces of the neighboring particles
vec3 calculateInternalForces();

//Calculates the speed of a neighboring particle from the position textures and uses it it to compute the force
//on the current particle 
vec3 getForceFromTexture(vec3 accumulated_force, vec3 current_pos, vec3 current_speed, vec2 texcoords, float r);

//Calculates the force from 1 neighbor
vec3 neighborForce(vec3 current_pos, vec3 current_speed, vec3 neighbor_pos, vec3 neighbor_speed, float r);

/*** MAIN ***/
void main()
{
    float pinned = texture(positionTexture, TexCoords).w;
    vec3 current_pos = texture(positionTexture, TexCoords).rgb;
    vec3 old_pos = texture(oldpositionTexture, TexCoords).rgb;

    vec3 current_speed = (current_pos - old_pos) / dt;

    vec3 F = calculateInternalForces();

    float mass = 0.1;
    vec3 gravity = vec3(0.0f, -0.981f, 0.0f); //simple gravitational pull
    vec3 wind = vec3(0.0f, 0.0f, 0.9f);
    
    F += gravity * mass + (current_speed * mass * damping); //Add default damping on momentum

    vec3 acceleration = (F / mass);

    //Euler integration
    // vec3 speed = current_speed + acceleration * dt;
    // vec3 pos = current_pos + speed * dt;
    
    //Verlet integration
    vec3 pos = 2 * current_pos - old_pos + acceleration * dt * dt;

    //collision detection
    if(!hideSphere) {
        vec3 dist_to_sphere = pos - center;
        if(length(dist_to_sphere) <= radius + 0.015 ) {
            pos = current_pos;
        }
    }
    

    //check if the point is pinned. If it is, do not move it
    if(pinned == 1.0) {
        pos = current_pos;
    }

	FragColor = vec4(pos, pinned);
} 


/*** FUNCTION IMPLEMENTATIONS ***/

//Calculate the internal force by accumulating the forces of the neighboring particles
vec3 calculateInternalForces() {

    //Current position and velocity
    vec3 current_pos = texture(positionTexture, TexCoords).rgb;
    vec3 old_pos = texture(oldpositionTexture, TexCoords).rgb;
    vec3 current_speed = (old_pos - current_pos) / dt;

    //compute neighbouring texture coordinates
    vec2 right = TexCoords + vec2(texture_offset_x, 0.0f);
    vec2 left = TexCoords - vec2(texture_offset_x, 0.0f);
    vec2 up = TexCoords + vec2(0.0f, texture_offset_y);
    vec2 down = TexCoords - vec2(0.0f, texture_offset_y);

    //1. the 4-Neighborhood (i.e. the 4 closest neighbors)
    vec3 force1 = vec3(0.0, 0.0, 0.0);

    force1 = getForceFromTexture(force1, current_pos, current_speed, right, rest_distances.x);
    force1 = getForceFromTexture(force1, current_pos, current_speed, left, rest_distances.x);
    force1 = getForceFromTexture(force1, current_pos, current_speed, up, rest_distances.y);
    force1 = getForceFromTexture(force1, current_pos, current_speed, down, rest_distances.y);

    //2. the 8-Neighborhood (i.e. the 8 closest neighbors)
    //compute neighbouring texture coordinates
    vec2 up_right = TexCoords + vec2(texture_offset_x, texture_offset_y);
    vec2 down_left = TexCoords + vec2(-texture_offset_x, -texture_offset_y);
    vec2 up_left = TexCoords + vec2(-texture_offset_x, texture_offset_y);
    vec2 down_right = TexCoords + vec2(texture_offset_x, -texture_offset_y);

    vec3 force2 = vec3(0.0, 0.0, 0.0);

    force2 = getForceFromTexture(force2, current_pos, current_speed, up_right, rest_distances.z);
    force2 = getForceFromTexture(force2, current_pos, current_speed, up_left, rest_distances.z);
    force2 = getForceFromTexture(force2, current_pos, current_speed, down_right, rest_distances.z);
    force2 = getForceFromTexture(force2, current_pos, current_speed, down_left, rest_distances.z);
    

    //3. the 12-Neighborhood (i.e. the 12 closest neighbors)
    vec3 force3 = vec3(0.0, 0.0, 0.0);
    
    vec2 right2 = TexCoords + vec2(2 * texture_offset_x, 0.0f);
    vec2 left2 = TexCoords - vec2(2 * texture_offset_x, 0.0f);
    vec2 up2 = TexCoords + vec2(0.0f, 2 * texture_offset_y);
    vec2 down2 = TexCoords - vec2(0.0f, 2 * texture_offset_y);

    force3 = getForceFromTexture(force3, current_pos, current_speed, right2, 2.0 * rest_distances.x);
    force3 = getForceFromTexture(force3, current_pos, current_speed, left2, 2.0 * rest_distances.x);
    force3 = getForceFromTexture(force3, current_pos, current_speed, up2, 2.0 * rest_distances.y);
    force3 = getForceFromTexture(force3, current_pos, current_speed, down2, 2.0 * rest_distances.y);

    vec3 result_force = force1 + force2 + force3;
    return result_force;
}

vec3 getForceFromTexture(vec3 accumulated_force, vec3 current_pos, vec3 current_speed, vec2 texcoords, float r){
     
    if(texcoords.x > 0 && texcoords.x < 1 && texcoords.y > 0 && texcoords.y < 1) {
        vec3 neighbor_pos = texture(positionTexture, texcoords).rgb;
        vec3 old_neighbor_pos = texture(oldpositionTexture, texcoords).rgb;
        vec3 neighbor_speed = (neighbor_pos - old_neighbor_pos) / dt;
        accumulated_force += neighborForce(current_pos, current_speed, neighbor_pos, neighbor_speed, r);
    }
    
    return accumulated_force;
}

vec3 neighborForce(vec3 current_pos, vec3 current_speed, vec3 neighbor_pos, vec3 neighbor_speed, float r) {
    //Calculate distance to the neighbor
    vec3 dist = current_pos - neighbor_pos;

    vec3 speed_diff = current_speed - neighbor_speed; //speed difference between neighboring particles, used for damping
    
    float ks = 80.75, kd = -0.08f; //constants


    vec3 internal_force = -(ks * (length(dist) - r) + (kd * (dot(dist, speed_diff) / length(dist)))) * (dist / length(dist));
    //vec3 internal_force = vec3(0.4, 0.2, 0.7);
    return internal_force;
}