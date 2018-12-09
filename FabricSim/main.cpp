#define  GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"


#include "Shader.h"
#include "Fabric.h"
#include "Camera.h"

#include "GL_utilities.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;


// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


/***** Declare Framebuffer objects. ******/
//We're gonna need 4 of them for pingponging
//2 for positions, 2 for velocites
FBOstruct *fbo1, *fbo2, *fbo3, *fbo4;

Shader plainShader, velocityShader, positionShader, testShader;


/*** Screen quad ***/
float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
						 // positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	1.0f, -1.0f,  1.0f, 0.0f,
	1.0f,  1.0f,  1.0f, 1.0f
};

GLuint quadVAO;


/***** The size of the fabric in particles *****/
GLsizei num_particles_width = 20;
GLsizei num_particles_height = 20;


/***** Function Declarations *****/
GLuint generateTextureFromData(GLfloat * data);

void updatePositions(FBOstruct * pos1, FBOstruct * pos2, FBOstruct * vel1, FBOstruct * vel2);

void drawTextureToFBO(FBOstruct *fbo, GLuint texture);
void drawScreenQuad(Shader shader);


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Fabric Simulator", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	/********** set up screen quad **********/
	// screen quad VAO
	GLuint quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	/*********** Model view and projection matrices ************/



	/************** Create and compile shaders *****************/
	//Shader plainShader("plaintextureshader.vert", "plaintextureshader.frag");
	plainShader.init("shaders/plaintextureshader.vert", "shaders/plaintextureshader.frag");
	//Shader phongShader("shaders/phong.vert", "shaders/phong.frag");
	velocityShader.init("shaders/velocity.vert", "shaders/velocity.frag");
	positionShader.init("shaders/position.vert", "shaders/position.frag");
	testShader.init("shaders/testshader.vert", "shaders/testshader.frag");

	//Here is one way we can bind a texture to the shader
	//This will probably be more relevant when the shader needs more than one texture
	//plainShader.use();
	//plainShader.setInt("screenTexture", 0);

	
	/************** Create Fabric and position textures *****************/
	Fabric f{ 1, 1, num_particles_width, num_particles_height };
	f.Create_Fabric();

	//We must create textures from the position data in Fabric.
	GLuint position_texture1 = generateTextureFromData(f.positionarray);
	GLuint position_texture2 = generateTextureFromData(f.positionarray);

	//Create textures containing velocities, initially zero
	GLuint velocity_texture1 = generateTextureFromData(f.velocityarray);
	GLuint velocity_texture2 = generateTextureFromData(f.velocityarray);


	/*********** set up fram buffer objects *****************/
	fbo1 = initFBO(SCR_WIDTH, SCR_HEIGHT, 0);
	fbo2 = initFBO(SCR_WIDTH, SCR_HEIGHT, 0);
	fbo3 = initFBO(SCR_WIDTH, SCR_HEIGHT, 0);
	fbo4 = initFBO(SCR_WIDTH, SCR_HEIGHT, 0);

	drawTextureToFBO(fbo1, position_texture1);
	drawTextureToFBO(fbo2, position_texture2);
	drawTextureToFBO(fbo3, velocity_texture1);
	drawTextureToFBO(fbo4, velocity_texture2);
	useFBO(0L, fbo1, 0L);

	
	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	int flip = 0;

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{

		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		

		//Every other turn, ping pong to the other buffer
		if (flip == 0) {
			updatePositions(fbo1, fbo2, fbo3, fbo4);
			flip = 1;
			useFBO(0L, fbo1, 0L);
		}
		else {
			updatePositions(fbo2, fbo1, fbo4, fbo3);
			flip = 0;
			useFBO(0L, fbo2, 0L);
		}

		///*useFBO(fbo1, fbo2, 0L);
		//f.render();*/

		//useFBO(0L, fbo1, 0L);

		//plainShader.use();
		////glBindTexture(GL_TEXTURE_2D, position_texture1); //just for testing: bind the position texture to the screen quad
		//glBindVertexArray(quadVAO);
		//glDrawArrays(GL_TRIANGLES, 0, 6);

		/*** TEST: Apply a texture from fbo to fabric.***/
		//Update the positions of the fabric
		testShader.use();

		glClearColor(0.7f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		useFBO(0L, fbo1, 0L);

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		testShader.setMat4("projection", projection);

		// camera/view transformation
		glm::mat4 view = camera.GetViewMatrix();
		testShader.setMat4("view", view);


		glm::mat4 model(1.0f);
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		testShader.setMat4("model", model);


		f.render();

		
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	/*glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);*/

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	// Close the OpenGL window and terminate GLFW.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}


// generate texture
GLuint generateTextureFromData(GLfloat * data) {
	GLuint textureID = 0;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, num_particles_width, num_particles_height, 0, GL_RGB, GL_FLOAT, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	return textureID;
}

// Draw a texture to the fbo (for initialization purposes)
void drawTextureToFBO(FBOstruct *fbo, GLuint texture) {
	useFBO(fbo, 0L, 0L); // Render to the fbo;

	plainShader.use();
	glBindTexture(GL_TEXTURE_2D, texture); //just for testing: bind the position texture to the screen quad
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawScreenQuad(Shader shader) {
	shader.use();
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}


//Here we do pingponging
void updatePositions(FBOstruct * pos1, FBOstruct * pos2, FBOstruct * vel1, FBOstruct * vel2) {

	// 1. render position_texture1 to position_texture2, with velocity_texture1 as additional input.
	positionShader.use();
	glUniform1i(glGetUniformLocation(positionShader.ID, "oldpositionTexture"), 0);
	glUniform1i(glGetUniformLocation(positionShader.ID, "velocityTexture"), 1);

	useFBO(pos2, pos1, vel1); //Render to fbo1, without any input
	
	drawScreenQuad(positionShader); //draw the texture

	// 2. render velocity_texture1 to velocity_texture2, updating the velocity for the next pass.
	//useFBO(fbo2, fb01, 0L);
	velocityShader.use();
	glUniform1i(glGetUniformLocation(velocityShader.ID, "oldVelocityTexture"), 0);
	glUniform1i(glGetUniformLocation(velocityShader.ID, "positionTexture"), 1); // Need the positions to be able to update the velocity

	useFBO(vel2, vel1, pos1);
	drawScreenQuad(velocityShader);

}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}