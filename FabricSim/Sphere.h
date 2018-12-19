#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
class Sphere {

private:

	// All data members are private. They are accessed only by methods in the class.
	GLuint vao;          // Vertex array object, the main handle for geometry
	int nverts; // Number of vertices in the vertex array
	int ntris;  // Number of triangles in the index array (may be zero)
	GLuint vertexbuffer; // Buffer ID to bind to GL_ARRAY_BUFFER
	GLuint indexbuffer;  // Buffer ID to bind to GL_ELEMENT_ARRAY_BUFFER
	GLfloat *vertexarray; // Vertex array on interleaved format: x y z nx ny nz s t
	GLuint *indexarray;   // Element index array

public:
	/* Constructor: initialize a triangleSoup object to all zeros */
	Sphere();

	/* Destructor: clean up allocated data in a triangleSoup object */
	~Sphere();

	/* Clean up allocated data in a triangleSoup object */
	void clean();

	/* Create a sphere (approximated by polygon segments) */
	void createSphere(float radius, int segments);

	/* Render the geometry in a triangleSoup object */
	void render();
};