#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GL_utilities.h"
#include "Sphere.h"
#include "Shader.h"
#include "Fabric.h"
#include "Camera.h"



/**** Settings ****/
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0f, -1.0f, 4.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// world rotation
glm::mat4 rot(1.0);

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


/***** Declare Framebuffer objects and shaders. ******/
FBOstruct *fbo1, *fbo2, *fbo3, *fbo4;

Shader plainShader, positionShader2, testShader, sphereShader;


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
float fabric_width = 2;
float fabric_height = 2;
GLsizei num_particles_width = 50;
GLsizei num_particles_height = 50;

bool wireframe_mode = false;


/***** Sphere variables *******/
float radius = 0.2f;
glm::vec3 center(0.0f, 0.0f, 0.0f);
Sphere sphere;


/***** Function Declarations *****/
GLuint generateTextureFromData(GLfloat * data);
void updatePositionsVerlet(FBOstruct * pos1, FBOstruct * pos2, FBOstruct * pos3);
void drawTextureToFBO(FBOstruct *fbo, GLuint texture);
void drawScreenQuad(Shader shader);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);


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


	/************** Create and compile shaders *****************/
	plainShader.init("../shaders/plaintextureshader.vert", "../shaders/plaintextureshader.frag");
	positionShader2.init("../shaders/position2.vert", "../shaders/position2.frag");
	testShader.init("../shaders/testshader.vert", "../shaders/testshader.frag");
	sphereShader.init("../shaders/plain.vert", "../shaders/plain.frag");
	
	/************** Create Fabric and position textures *****************/
	Fabric f{ fabric_width, fabric_height, num_particles_width, num_particles_height };
	f.Set_Pinned(AllCorners);
	f.Create_Fabric();

	// Create textures from the position data in Fabric.
	GLuint position_texture1 = generateTextureFromData(f.positionarray);
	GLuint position_texture2 = generateTextureFromData(f.positionarray);
	GLuint position_texture3 = generateTextureFromData(f.positionarray);

	//Create texture with stripes
	GLuint checker_texture = generateTextureFromData(f.texturearray);

	/***** Pass texture offsets to position shader *******/
	float offset_x = 1 / (float)num_particles_width;
	float offset_y = 1 / (float)num_particles_height;

	//Calculate rest distances for horizontal, vertical and diagonal neighbors
	float rest_dist_x = fabric_width / (float)(num_particles_width - 1);
	float rest_dist_y = fabric_height / (float)(num_particles_height - 1);
	float rest_dist_d = sqrt(rest_dist_x*rest_dist_x + rest_dist_y * rest_dist_y);

	glm::vec3 rest_distances(rest_dist_x, rest_dist_y, rest_dist_d);

	//when using verlet, pass to position shader 2
	positionShader2.use();
	glUniform1f(glGetUniformLocation(positionShader2.ID, "texture_offset_x"), offset_x);
	glUniform1f(glGetUniformLocation(positionShader2.ID, "texture_offset_y"), offset_y);
	glUniform3fv(glGetUniformLocation(positionShader2.ID, "rest_distances"), 1, glm::value_ptr(rest_distances));

	/*********** set up frame buffer objects *****************/
	fbo1 = initFBO(SCR_WIDTH, SCR_HEIGHT, 0);
	fbo2 = initFBO(SCR_WIDTH, SCR_HEIGHT, 0);
	fbo3 = initFBO(SCR_WIDTH, SCR_HEIGHT, 0);
	fbo4 = initFBO(SCR_WIDTH, SCR_HEIGHT, 0);

	drawTextureToFBO(fbo1, position_texture1);
	drawTextureToFBO(fbo2, position_texture2);
	drawTextureToFBO(fbo3, position_texture3);
	drawTextureToFBO(fbo4, checker_texture); // stripes
	
	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Create a sphere	
	sphere.createSphere(radius, 20);

	
	/***** Render loop *****/
	int flip = 0;
	while (!glfwWindowShouldClose(window))
	{

		// per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);

		//ping pong with verlet
		if (flip == 0) {
		updatePositionsVerlet(fbo1, fbo2, fbo3);
		flip = 1;
		}
		else {
		updatePositionsVerlet(fbo2, fbo1, fbo3);
		flip = 0;
		}

		glEnable(GL_DEPTH_TEST);

		// Create projection and view matrix to send to shader
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix(); // camera/view transformation

		// Draw sphere
		sphereShader.use();
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// pass projection matrix to the sphere shader
		sphereShader.setMat4("projection", projection);
		sphereShader.setMat4("view", view);

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = model * rot;
		sphereShader.setMat4("model", model);

		sphere.render();

		// Draw the fabric
		testShader.use();
		glUniform1i(glGetUniformLocation(testShader.ID, "positionTexture"), 0);
		glUniform1i(glGetUniformLocation(testShader.ID, "normalTexture"), 1);

		// Pass projection and modelview matrices to the fabric shader.
		testShader.setMat4("projection", projection);
		testShader.setMat4("view", view);
		glm::mat4 model2(1.0f);
		model2 = model2 * rot;
		testShader.setMat4("model", model2);

		//Toggle between regular and wireframe mode using key 'T'
		if (wireframe_mode) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			f.render();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else
		{
			f.render();
		}

		
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glDisable(GL_DEPTH_TEST);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// Close the OpenGL window and terminate GLFW.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

/******* Function implementations ********/

// Generate float textures from generated data
GLuint generateTextureFromData(GLfloat * data) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, num_particles_width, num_particles_height, 0, GL_RGBA, GL_FLOAT, data);
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

// Draw to quad covering the screen
void drawScreenQuad(Shader shader) {
	shader.use();
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

//Update the position textures using Verlet integration
void updatePositionsVerlet(FBOstruct * pos1, FBOstruct * pos2, FBOstruct * pos3) {
	
	// 1. Update pos1 with new positions using pos2(current positions) and pos3(old positions) as input
	positionShader2.use();
	glUniform1i(glGetUniformLocation(positionShader2.ID, "positionTexture"), 0);
	glUniform1i(glGetUniformLocation(positionShader2.ID, "oldpositionTexture"), 1);
	useFBO(pos1, pos2, pos3); //Render to pos1
	drawScreenQuad(positionShader2); //draw the texture

	// 2. update the fbo storing the old texture with the current texture
	plainShader.use();
	useFBO(pos3, pos2, 0L);
	drawScreenQuad(plainShader);

	//draw to screen, using the positions and stripe textures as input.
	useFBO(0L, pos1, fbo4);
}



// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------

//Update world rotation matrix
void updateRot(int dir) {
	float timer = deltaTime * dir;
	rot = glm::rotate(rot, glm::radians(timer * 100), glm::vec3(0.0f, 1.0f, 0.0f));

}

// Process input from keyboard
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
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		updateRot(-1);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		updateRot(1);
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		wireframe_mode = abs(wireframe_mode - 1);
		
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
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}