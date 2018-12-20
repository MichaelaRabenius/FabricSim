#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Enum to more easily set pinned portions of the fabric from main using the function Fabric::Set_Pinned()
enum Pinned {
	None,
	UpperLeftCorner,
	UpperRightCorner,
	LowerLeftCorner,
	LowerRightCorner,
	UpperCorners,
	LowerCorners,
	AllCorners,
	UpperEdge,
	LowerEdge,
	RightEdge,
	LeftEdge,
	Diagonal,
};

// class to represent the fabric
class Fabric {
public:
	// This is an important constructor for the entire system of particles and constraints
	Fabric(float width, float height, int num_particles_width, int num_particles_height);
	~Fabric();
	
	// Function to set up the fabric
	void Create_Fabric();

	// Clean up allocated data
	void clean();

	// Draw the fabric
	void render();

	//Set which particles should be pinned
	void Set_Pinned(Pinned pin);

	//Compute which portions of the fabrics should be pinned
	void Compute_Pinned_Values(int x, int y, int idx);

	//Data arrays for generating textures
	GLfloat *positionarray;
	GLfloat *texturearray;

private:
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

	//Pinned particles
	Pinned pinned_particles;
 };
