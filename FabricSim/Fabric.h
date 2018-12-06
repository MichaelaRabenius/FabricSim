#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// class to represent the fabric
class Fabric {
public:
	/* This is a important constructor for the entire system of particles and constraints*/
	Fabric(float width, float height, int num_particles_width, int num_particles_height);
	~Fabric();

	void Create_Fabric();

	/* Clean up allocated data in a triangleSoup object */
	void clean();

	/* Draw the fabric */
	void render();

	float width; 
	float height;
	int num_particles_width; // number of particles in "width" direction
	int num_particles_height; // number of particles in "height" direction
							  // total number of particles is num_particles_width*num_particles_height

	// For rendering
	GLuint vao;          // Vertex array object, the main handle for geometry
	int nverts; // Number of vertices in the vertex array
	int ntris;  // Number of triangles in the index array (may be zero)
	GLuint vertexbuffer; // Buffer ID to bind to GL_ARRAY_BUFFER
	GLuint indexbuffer;  // Buffer ID to bind to GL_ELEMENT_ARRAY_BUFFER
	GLfloat *vertexarray; // Vertex array on interleaved format: x y z 
	GLuint *indexarray;   // Element index array

	GLfloat *positionarray;
	GLfloat *velocityarray;

	void printError(const char *errtype, const char *errmsg);
	

 };
