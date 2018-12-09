#include "Fabric.h"
#include <glm/glm.hpp>
#include <iostream>
Fabric::Fabric(float width, float height, int num_particles_width, int num_particles_height) 
	: width(width), height(height), num_particles_width(num_particles_width), num_particles_height(num_particles_height) {
	
	/*Initialize variables for rendering to zero*/
	vao = 0;
	vertexbuffer = 0;
	indexbuffer = 0;
	vertexarray = NULL;
	indexarray = NULL;
	nverts = 0;
	ntris = 0;
	positionarray = NULL;
	velocityarray = NULL;
	/********************************************/
}

Fabric::~Fabric()
{
	clean();
}

void Fabric::Create_Fabric()
{
	// Delete any previous content
	clean();

	nverts = num_particles_width * num_particles_width;
	ntris = (num_particles_width - 1)*(num_particles_height - 1) * 2;

	vertexarray = new GLfloat[8 * nverts]; // we need 8 values for position, normal and texture coordinates
	indexarray = new GLuint[3 * ntris];

	positionarray = new GLfloat[3 * nverts];
	velocityarray = new GLfloat[3 * nverts];

	int idx = 0;
	int idx2 = 0;
	//particles.resize(num_particles_height * num_particles_width);
	//Create the particles
	for (int x = 0; x < num_particles_width; ++x) {
		for (int y = 0; y < num_particles_height; ++y) {

			// Calculate the position of the new particle.
			glm::vec3 position = glm::vec3(width * (x / (float)num_particles_width) - 0.5f, height * (y / (float)num_particles_height) - 0.5f, 0.0f);
			
			//Insert vertex positions
			vertexarray[idx] = position.x;
			vertexarray[idx + 1] = position.y;
			vertexarray[idx + 2] = position.z;

			positionarray[idx2] = position.x;
			positionarray[idx2 + 1] = position.y;
			positionarray[idx2 + 2] = position.z;

			//Insert normal (always pointing in positive z- direction)
			vertexarray[idx + 3] = 0.0f;
			vertexarray[idx + 4] = 0.0f;
			vertexarray[idx + 5] = 1.0f;

			//Insert texture coordinates
			vertexarray[idx + 6] = x / (float)num_particles_width;
			vertexarray[idx + 7] = y / (float)num_particles_height;
			
			int a = 0;
			//Insert initial velocities for each particle
			//Initially we start at velocity = 0
			velocityarray[idx2] = 0.0f;
			velocityarray[idx2 + 1] = 0.0f;
			velocityarray[idx2 + 2] = 0.0f;

			// insert texture coordinates
			/*velocityarray[idx + 3] = x / (float)num_particles_width;
			velocityarray[idx + 4] = y / (float)num_particles_height;*/



			idx += 8;
			idx2 += 3;

		}
	}

	idx = 0;
	for (int x = 0; x < num_particles_width - 1; x++)
	{
		for (int y = 0; y < num_particles_height - 1; y++)
		{

			int first = y * num_particles_height + x;
			indexarray[idx++] = y * num_particles_height + x;
			indexarray[idx++] = y * num_particles_height + x + 1;
			indexarray[idx++] = (y + 1) * num_particles_height + x + 1;

			indexarray[idx++] = y * num_particles_height + x;
			indexarray[idx++] = (y + 1) * num_particles_height + x + 1;
			indexarray[idx++] = (y + 1) * num_particles_height + x;
			
		}
	}

	// Generate one vertex array object (VAO) and bind it
	glGenVertexArrays(1, &(vao));
	glBindVertexArray(vao);

	// Generate two buffer IDs
	glGenBuffers(1, &vertexbuffer);
	glGenBuffers(1, &indexbuffer);

	// Activate the vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Present our vertex coordinates to OpenGL
	glBufferData(GL_ARRAY_BUFFER, 8 * nverts * sizeof(GLfloat), vertexarray, GL_STATIC_DRAW);

	// Specify how many attribute arrays we have in our VAO
	glEnableVertexAttribArray(0); // Vertex coordinates
	glEnableVertexAttribArray(1); // Normals
	glEnableVertexAttribArray(2); // Texture coordinates

	// Specify how OpenGL should interpret the vertex buffer data:
	// Attributes 0, 1, 2 (must match the lines above and the layout in the shader)
	// Number of dimensions (3 means vec3 in the shader, 2 means vec2)
	// Type GL_FLOAT
	// Not normalized (GL_FALSE)
	// Stride 8 (interleaved array with 8 floats per vertex)
	// Array buffer offset 0, 3, 6 (offset into first vertex)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,	8 * sizeof(GLfloat), (void*)0); // xyz coordinates
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // normals
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,	8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat))); // texcoords


	// Activate the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
	// Present our vertex indices to OpenGL
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		3 * ntris * sizeof(GLuint), indexarray, GL_STATIC_DRAW);

	// Deactivate (unbind) the VAO and the buffers again.
	// Do NOT unbind the buffers while the VAO is still bound.
	// The index buffer is an essential part of the VAO state.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}



void Fabric::clean()
{
	if (glIsVertexArray(vao)) {
		glDeleteVertexArrays(1, &vao);
	}
	vao = 0;

	if (glIsBuffer(vertexbuffer)) {
		glDeleteBuffers(1, &vertexbuffer);
	}
	vertexbuffer = 0;

	if (glIsBuffer(indexbuffer)) {
		glDeleteBuffers(1, &indexbuffer);
	}
	indexbuffer = 0;

	if (vertexarray) {
		delete[] vertexarray;
	}
	if (indexarray) {
		delete[] indexarray;
	}
	if (velocityarray) {
		delete[] velocityarray;
	}

	nverts = 0;
	ntris = 0;
}

void Fabric::render()
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 3 * ntris, GL_UNSIGNED_INT, (void*)0);
	// (mode, vertex count, type, element array buffer offset)
	glBindVertexArray(0);
}

/*
* private
* printError() - Signal an error.
* Simple printf() to console for portability.
*/
void Fabric::printError(const char *errtype, const char *errmsg) {
	fprintf(stderr, "%s: %s\n", errtype, errmsg);
}
