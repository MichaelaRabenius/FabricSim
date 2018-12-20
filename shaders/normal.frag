#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D positionTexture;

uniform float texture_offset_x;
uniform float texture_offset_y;


vec3 calculateNormalCenter(vec3 pos); //for all center points
vec3 calculateNormalRight(vec3 pos); //normal along right edge
vec3 calculateNormalLeft(vec3 pos); //normal along left edge
vec3 calculateNormalUp(vec3 pos); //normal along upper edge
vec3 calculateNormalDown(vec3 pos); //normal along lower edge
vec3 calculateNormalUpperLeft(vec3 pos); //normal in upperleft corner
vec3 calculateNormalLowerRight(vec3 pos); //normal in lower right corner
vec3 calculateNormalLowerLeft(vec3 pos); //normal in lower left corner
vec3 calculateNormalUpperRight(vec3 pos); //normal in upper right corner


void main()
{
    vec4 position = texture(positionTexture, TexCoords);
    vec3 normal = vec3(0.0, 0.0, 0.0);

    if(position.w == 0.0) {
        normal = calculateNormalCenter(position.rgb);
    }
    else if(position.w == 1.0){
        normal = calculateNormalUp(position.rgb);
    }
    else if(position.w == 2.0){
        normal = calculateNormalRight(position.rgb);
    }
    else if(position.w == 3.0){
        normal = calculateNormalDown(position.rgb);
    }
    else if(position.w == 4.0){
        normal = calculateNormalLeft(position.rgb);
    }

    // if(normal != vec3(0.0, 0.0, 0.0)){
    //     normal = vec3(1.0, 0.0, 0.0);
    // }
    
    FragColor = vec4(normal, 1.0);
} 


//for all center points
vec3 calculateNormalCenter(vec3 pos){
    vec2 coordinates[6]; 
    coordinates[0] = TexCoords - vec2(texture_offset_x, 0.0f); //left
    coordinates[1] = TexCoords + vec2(0.0f, texture_offset_y); //up
    coordinates[2] = TexCoords + vec2(texture_offset_x, texture_offset_y); //upright
    coordinates[3] = TexCoords + vec2(texture_offset_x, 0.0f); // right
    coordinates[4] = TexCoords - vec2(0.0f, texture_offset_y); // down
    coordinates[5] = TexCoords + vec2(-texture_offset_x, -texture_offset_y); //downleft

    //vec3 vertices[6];
    vec3 total_normal = vec3(0.0, 0.0, 0.0);
    for(int i = 0; i < 6; i++){
        vec3 p1 = texture(positionTexture, coordinates[i]).rgb;
        vec3 p2 = vec3(0.0, 0.0, 0.0);
        if(i == 5){
           p2 = texture(positionTexture, coordinates[0]).rgb;
        } else {
            p2 = texture(positionTexture, coordinates[i+1]).rgb;
        }
                
        vec3 v1 = p1 - pos;
        vec3 v2 = p2 - pos;
        
        vec3 n = cross(v1, v2);
        float A = length(n) * 0.5;
        vec3 n_normalized = n / length(n);
        total_normal = n_normalized;
    }
    total_normal = total_normal / length(total_normal);
    return total_normal;
}

//normal along right edge
vec3 calculateNormalRight(vec3 pos){
    vec2 coordinates[4];
    coordinates[0] = TexCoords - vec2(0.0f, texture_offset_y); // down
    coordinates[1] = TexCoords + vec2(-texture_offset_x, -texture_offset_y); //downleft
    coordinates[2] = TexCoords - vec2(texture_offset_x, 0.0f); //left
    coordinates[3] = TexCoords + vec2(0.0f, texture_offset_y); //up

    vec3 total_normal = vec3(0.0, 0.0, 0.0);
    for(int i = 0; i < 3; i++){
        vec3 p1 = texture(positionTexture, coordinates[i]).rgb;
        vec3 p2 = texture(positionTexture, coordinates[i+1]).rgb;
                  
        vec3 v1 = p1 - pos;
        vec3 v2 = p2 - pos;
        
        vec3 n = cross(v1, v2);
        float A = length(n) * 0.5;
        vec3 n_normalized = n / length(n);
        total_normal = n_normalized;
    }
    total_normal = total_normal / length(total_normal);
    return total_normal;
}

//normal along left edge
vec3 calculateNormalLeft(vec3 pos){
   
    vec2 coordinates[4]; 
    coordinates[0] = TexCoords + vec2(0.0f, texture_offset_y); //up
    coordinates[1] = TexCoords + vec2(texture_offset_x, texture_offset_y); //upright
    coordinates[2] = TexCoords + vec2(texture_offset_x, 0.0f); // right
    coordinates[3] = TexCoords - vec2(0.0f, texture_offset_y); // down

    vec3 total_normal = vec3(0.0, 0.0, 0.0);
    for(int i = 0; i < 3; i++){
        vec3 p1 = texture(positionTexture, coordinates[i]).rgb;
        vec3 p2 = texture(positionTexture, coordinates[i+1]).rgb;
                  
        vec3 v1 = p1 - pos;
        vec3 v2 = p2 - pos;
        
        vec3 n = cross(v1, v2);
        float A = length(n) * 0.5;
        vec3 n_normalized = n / length(n);
        total_normal = n_normalized;
    }
    total_normal = total_normal / length(total_normal);
    return total_normal;
}

//normal along upper edge
vec3 calculateNormalUp(vec3 pos){
    vec2 coordinates[4]; 
    coordinates[0] = TexCoords + vec2(texture_offset_x, 0.0f); // right
    coordinates[1] = TexCoords - vec2(0.0f, texture_offset_y); // down
    coordinates[2] = TexCoords + vec2(-texture_offset_x, -texture_offset_y); //downleft
    coordinates[3] = TexCoords - vec2(texture_offset_x, 0.0f); //left  
    
    vec3 total_normal = vec3(0.0, 0.0, 0.0);
    for(int i = 0; i < 3; i++){
        vec3 p1 = texture(positionTexture, coordinates[i]).rgb;
        vec3 p2 = texture(positionTexture, coordinates[i+1]).rgb;
                  
        vec3 v1 = p1 - pos;
        vec3 v2 = p2 - pos;
        
        vec3 n = cross(v1, v2);
        float A = length(n) * 0.5;
        vec3 n_normalized = n / length(n);
        total_normal = n_normalized;
    }
    total_normal = total_normal / length(total_normal);
    return total_normal;
}

//normal along lower edge
vec3 calculateNormalDown(vec3 pos){
    vec2 coordinates[4]; 
    coordinates[0] = TexCoords - vec2(texture_offset_x, 0.0f); //left
    coordinates[1] = TexCoords + vec2(0.0f, texture_offset_y); //up
    coordinates[2] = TexCoords + vec2(texture_offset_x, texture_offset_y); //upright
    coordinates[3] = TexCoords + vec2(texture_offset_x, 0.0f); // right
    
    vec3 total_normal = vec3(0.0, 0.0, 0.0);
    for(int i = 0; i < 3; i++){
        vec3 p1 = texture(positionTexture, coordinates[i]).rgb;
        vec3 p2 = texture(positionTexture, coordinates[i+1]).rgb;
                  
        vec3 v1 = p1 - pos;
        vec3 v2 = p2 - pos;
        
        vec3 n = cross(v1, v2);
        float A = length(n) * 0.5;
        vec3 n_normalized = n / length(n);
        total_normal = n_normalized;
    }
    total_normal = total_normal / length(total_normal);
    return total_normal;
}

//normal in upperleft corner
vec3 calculateNormalUpperLeft(vec3 pos){
    vec2 coordinates[2];
    coordinates[0] = TexCoords + vec2(texture_offset_x, 0.0f); // right
    coordinates[1] = TexCoords - vec2(0.0f, texture_offset_y); // down
    
    
    vec3 total_normal = vec3(0.0, 0.0, 0.0);
    vec3 p1 = texture(positionTexture, coordinates[0]).rgb;
    vec3 p2 = texture(positionTexture, coordinates[1]).rgb;
                
    vec3 v1 = p1 - pos;
    vec3 v2 = p2 - pos;
    
    vec3 n = cross(v1, v2);
    float A = length(n) * 0.5;
    vec3 n_normalized = n / length(n);
    total_normal = n_normalized;

    total_normal = total_normal / length(total_normal);
    return total_normal;
}

//normal in lower right corner
vec3 calculateNormalLowerRight(vec3 pos){
    vec2 coordinates[2];
    coordinates[0] = TexCoords - vec2(texture_offset_x, 0.0f); //left
    coordinates[1] = TexCoords + vec2(0.0f, texture_offset_y); //up
    
    vec3 total_normal = vec3(0.0, 0.0, 0.0);
    vec3 p1 = texture(positionTexture, coordinates[0]).rgb;
    vec3 p2 = texture(positionTexture, coordinates[1]).rgb;
                
    vec3 v1 = p1 - pos;
    vec3 v2 = p2 - pos;
    
    vec3 n = cross(v1, v2);
    float A = length(n) * 0.5;
    vec3 n_normalized = n / length(n);
    total_normal = n_normalized;

    total_normal = total_normal / length(total_normal);
    return total_normal;
}

//normal in lower left corner
vec3 calculateNormalLowerLeft(vec3 pos){
    vec2 coordinates[3]; 
    coordinates[0] = TexCoords + vec2(0.0f, texture_offset_y); //up
    coordinates[1] = TexCoords + vec2(texture_offset_x, texture_offset_y); //upright
    coordinates[2] = TexCoords + vec2(texture_offset_x, 0.0f); // right
    
    vec3 total_normal = vec3(0.0, 0.0, 0.0);
    for(int i = 0; i < 2; i++){
        vec3 p1 = texture(positionTexture, coordinates[i]).rgb;
        vec3 p2 = texture(positionTexture, coordinates[i+1]).rgb;
                  
        vec3 v1 = p1 - pos;
        vec3 v2 = p2 - pos;
        
        vec3 n = cross(v1, v2);
        float A = length(n) * 0.5;
        vec3 n_normalized = n / length(n);
        total_normal = n_normalized;
    }
    total_normal = total_normal / length(total_normal);
    return total_normal;
}

//normal in upper right corner
vec3 calculateNormalUpperRight(vec3 pos){
    vec2 coordinates[3]; 

    coordinates[0] = TexCoords - vec2(0.0f, texture_offset_y); // down
    coordinates[1] = TexCoords + vec2(-texture_offset_x, -texture_offset_y); //downleft
    coordinates[2] = TexCoords - vec2(texture_offset_x, 0.0f); //left
   
    vec3 total_normal = vec3(0.0, 0.0, 0.0);
    for(int i = 0; i < 2; i++){
        vec3 p1 = texture(positionTexture, coordinates[i]).rgb;
        vec3 p2 = texture(positionTexture, coordinates[i+1]).rgb;
                  
        vec3 v1 = p1 - pos;
        vec3 v2 = p2 - pos;
        
        vec3 n = cross(v1, v2);
        float A = length(n) * 0.5;
        vec3 n_normalized = n / length(n);
        total_normal = n_normalized;
    }
    total_normal = total_normal / length(total_normal);
    return total_normal;
}