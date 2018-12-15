#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>

//void printError(const char *functionName);
GLuint loadShaders(const char *vertFileName, const char *fragFileName);
GLuint loadShadersG(const char *vertFileName, const char *fragFileName, const char *geomFileName);
GLuint loadShadersGT(const char *vertFileName, const char *fragFileName, const char *geomFileName,
	const char *tcFileName, const char *teFileName);
void dumpInfo(void);

typedef struct
{
	GLuint texid;
	GLuint fb;
	GLuint rb;
	GLuint depth;
	int width, height;
} FBOstruct;

FBOstruct *initFBO(int width, int height, int int_method);
FBOstruct *initFBO2(int width, int height, int int_method, int create_depthimage);
void useFBO(FBOstruct *out, FBOstruct *in1, FBOstruct *in2);
void useFBO(FBOstruct *out, FBOstruct *in1, FBOstruct *in2, FBOstruct *in3);
void useFBO(FBOstruct *out, FBOstruct *in1, FBOstruct *in2, FBOstruct *in3, FBOstruct *in4);
void updateScreenSizeForFBOHandler(int w, int h); // Temporary workaround to inform useFBO of screen size changes
