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
	texturearray = NULL;
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

	positionarray = new GLfloat[4 * nverts];
	texturearray = new GLfloat[4 * nverts];

	int idx = 0;
	int idx2 = 0;
	//particles.resize(num_particles_height * num_particles_width);
	//Create the particles
	int black = 0;
	for (int y = 0; y < num_particles_height; ++y) {
		for (int x = 0; x < num_particles_width; ++x) {

			// Calculate the position of the new particle.
			glm::vec3 position = glm::vec3(x * width / ((float)num_particles_width - 1) -(width*0.5f) ,0.0f, y * height / ((float)num_particles_height - 1) - (height*0.5f));
			
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
			vertexarray[idx + 6] = x / (float)(num_particles_width) + (1 / (float)(num_particles_width * 2));
			vertexarray[idx + 7] = y / (float)(num_particles_height) + (1 / (float)(num_particles_height * 2));

			// Compute which particles should be pinned
			Compute_Pinned_Values(x, y, idx2 + 3);
			
			//generate a black white texture
			if (black == 0) {
				texturearray[idx2] = 1.0f;
				texturearray[idx2 + 1] = 1.0f;
				texturearray[idx2 + 2] = 1.0f;
				texturearray[idx2 + 3] = 1.0f;
				if ((num_particles_width % 2 != 0) && x == num_particles_width - 1)
					black = 0;
				else 
					black = 1;
			} 
			else {
				texturearray[idx2] = 0.0f;
				texturearray[idx2 + 1] = 0.0f;
				texturearray[idx2 + 2] = 0.0f;
				texturearray[idx2 + 3] = 1.0f;
				if ((num_particles_width % 2 != 0) && x == num_particles_width - 1)
					black = 1;
				else
					black = 0;
			}


			idx += 8;
			idx2 += 4;

		}
	}

	idx = 0;
	for (int y = 0; y < num_particles_height - 1; y++)
	{
		for (int x = 0; x < num_particles_width - 1; x++)
		{

			//first triangle
			//3.  O
			//	  |\
			//	  | \
			//	  |  \
			//1.  O___O 2.

			indexarray[idx++] = y * num_particles_height + x; //lower left
			indexarray[idx++] = y * num_particles_height + x + 1; // lower right
			indexarray[idx++] = (y + 1) * num_particles_height + x; // upper left

			//second triangle
			//	1. O___O 3.
			//		\  |
			//		 \ |
			//		  \|
			//		2. O
			//

			int a = y * num_particles_height + x + 1;
			int b = (y + 1) * num_particles_height + x + 1;
			int c = (y + 1) * num_particles_height + x;

			indexarray[idx++] = y * num_particles_height + x + 1; //lower right
			indexarray[idx++] = (y + 1) * num_particles_height + x + 1; // upper right
			indexarray[idx++] = (y + 1) * num_particles_height + x; // upper left

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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * ntris * sizeof(GLuint), indexarray, GL_STATIC_DRAW);

	// Deactivate (unbind) the VAO and the buffers again.
	// Do NOT unbind the buffers while the VAO is still bound.
	// The index buffer is an essential part of the VAO state.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


// Deallocate data
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
	if (positionarray) {
		delete[] positionarray;
	}
	if (texturearray) {
		delete[] texturearray;
	}

	nverts = 0;
	ntris = 0;
}

// Draw the fabric
void Fabric::render()
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 3 * ntris, GL_UNSIGNED_INT, (void*)0);
	// (mode, vertex count, type, element array buffer offset)
	glBindVertexArray(0);
}

// Set which particles should be pinned
void Fabric::Set_Pinned(Pinned pin)
{
	pinned_particles = pin;
}

// Compute which particles should be pinned
void Fabric::Compute_Pinned_Values(int x, int y, int idx)
{
	positionarray[idx] = 0.0f;

	switch (pinned_particles) {
	case None:
		positionarray[idx] = 0.0f;
		break;
	case UpperLeftCorner:
		if ((x == 0 && y == num_particles_height - 1)) {
			positionarray[idx] = 1.0f;
		}
		break;
	case UpperRightCorner:
		if ((x == num_particles_width - 1 && y == num_particles_height - 1)) {
			positionarray[idx] = 1.0f;
		}
		break;
	case LowerLeftCorner:
		if (x == 0 && y == 0) {
			positionarray[idx] = 1.0f;
		}
		break;
	case LowerRightCorner:
		if (x == num_particles_width - 1 && y == 0) {
			positionarray[idx] = 1.0f;
		}
		break;
	case UpperCorners:
		if ((x == 0 && y == num_particles_height - 1) || (x == num_particles_width - 1 && y == num_particles_height - 1)) {
			positionarray[idx] = 1.0f;
		}
		break;
	case LowerCorners:
		if ((x == 0 && y == 0) || (x == num_particles_width - 1 && y == 0)) {
			positionarray[idx] = 1.0f;
		}
		break;
	case AllCorners:
		if ((x == 0 && y == num_particles_height - 1) || (x == num_particles_width - 1 && y == num_particles_height - 1) ||
			(x == 0 && y == 0) || (x == num_particles_width - 1 && y == 0)) {
			positionarray[idx] = 1.0f;
		}
		break;
	case UpperEdge:
		if (y == num_particles_height - 1) {
			positionarray[idx] = 1.0f;
		}
		break;
	case LowerEdge:
		if (y == 0) {
			positionarray[idx] = 1.0f;
		}
		break;
	case RightEdge:
		if (x == num_particles_width - 1) {
			positionarray[idx] = 1.0f;
		}
		break;
	case LeftEdge:
		if (x == 0) {
			positionarray[idx] = 1.0f;
		}
		break;
	case Diagonal:
		if ((x == 0 && y == num_particles_height - 1) || (x == num_particles_width - 1 && y == 0)) {
			positionarray[idx] = 1.0f;
		}
		break;
	default:
		positionarray[idx] = 0.0f;
	}
}
