#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 exSurface;

uniform sampler2D normalTexture;


void main()
{   
    // vec3 exNormal = texture(normalTexture, TexCoords).rgb;

    // const vec3 light = vec3(0.58, 0.58, 0.58); // Given in VIEW coordinates! You usually specify light sources in world coordinates.
	// float diffuse, specular, shade;

	// // Diffuse
	// diffuse = dot(normalize(exNormal), light);
	// diffuse = max(0.0, diffuse); // No negative light

	// // Specular
	// vec3 r = reflect(-light, normalize(exNormal));
	// vec3 v = normalize(-exSurface); // View direction
	// specular = dot(r, v);
	// if (specular > 0.0)
	// 	specular = 10.0 * pow(specular, 150.0);
	// specular = max(specular, 0.0);
	// shade = 0.7*diffuse + 1.0*specular;
	// FragColor = vec4(shade, shade, shade, 1.0);

    vec4 color = texture(normalTexture, TexCoords);

    vec4 icolor = vec4(1 - color.r, 1 - color.g, 1 - color.b, 1.0);
    FragColor = vec4(0.5, 0.1, 0.3, 1.0);
    FragColor = color;
} 