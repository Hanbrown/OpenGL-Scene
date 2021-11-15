/*
 * Name: Pranav Rao
 * Class: CSCI 168 Graphics Programming in OpenGL
 * Description: This header file includes several key functions.
 */

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL2\soil2.h>
#include <string>
#include <iostream>
#include <fstream>
#include <glm\glm.hpp>
#include <glm\ext.hpp>

// Check for Errors (1/3), copied from TB
void printShaderLog(GLuint shader) {
	int len = 0;
	int chWrittn = 0;
	char * log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char *)malloc(len);
		glGetShaderInfoLog(shader, len, &chWrittn, log);
		std::cout << "Shader Info Log: " << log << std::endl;
		free(log);
	}
}

// Check for Errors (2/3), copied from TB
void printProgramLog(int prog) {
	int len = 0;
	int chWrittn = 0;
	char * log;
	glGetShaderiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char *)malloc(len);
		glGetShaderInfoLog(prog, len, &chWrittn, log);
		std::cout << "Program Info Log: " << log << std::endl;
		free(log);
	}
}

// Check for Errors (3/3), copied from TB
bool checkOpenGLError() {
	bool foundError = false;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR) {
		std::cout << "glError: " << glErr << std::endl;
		foundError = true;
		glErr = glGetError();
	}
	return foundError;
}

// Make GLSL code a String
std::string readShaderSource(const char * filePath) {
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);
	std::string line = "";

	// Search for start-of-file marker
	std::getline(fileStream, line);
	while (line.find("#sof") == std::string::npos) {
		getline(fileStream, line);
	}

	// Read shader file until end-of-file marker
	getline(fileStream, line);
	while (line.find("#eof") == std::string::npos) {
		content.append(line + "\n");
		getline(fileStream, line);
	}
	fileStream.close();

	return content;
}

// Compile the GLSL code
GLuint createShaderProgram(const char * vshaderSrc, const char * fshaderSrc) {
	// For debugging
	GLint vertCompiled;
	GLint fragCompiled;
	GLint linked;

	// Get shader src code from GLSL file -- NOTE: file structure varies from that used in textbook
	std::string vertShaderStr = readShaderSource(vshaderSrc);
	std::string fragShaderStr = readShaderSource(fshaderSrc);

	// Make GLSL code a c-string
	const char * vertShaderSrc = vertShaderStr.c_str();
	const char * fragShaderSrc = fragShaderStr.c_str();

	// Generate the shaders
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Populate each shader with the string arrays
	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	// Compile vertex shader and check for error
	glCompileShader(vShader);
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1) {
		std::cout << "vertex compilation failed" << std::endl;
		printShaderLog(vShader);
	}

	// Compile fragment shader and check for error
	glCompileShader(fShader);
	checkOpenGLError();
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1) {
		std::cout << "fragment compilation failed" << std::endl;
		printShaderLog(fShader);
	}

	// Create OpenGL program (see page 15, P2)
	GLuint vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);
	glLinkProgram(vfProgram);

	// Check for linking errors
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1) {
		std::cout << "linking failed" << std::endl;
		printProgramLog(vfProgram);
	}

	return vfProgram;
}

// Copied from Text
GLuint loadTexture(const char *texImagePath) {
	GLuint textureID;
	textureID = SOIL_load_OGL_texture(texImagePath, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (textureID == 0)
		std::cout << "Could not find Texture File " << texImagePath << std::endl;
	return textureID;
}

// Load a Cube UV Map (Copied from Text)
GLuint loadCubeMap(const char * mapDir) {
	GLuint textureRef;

	// Assume that all 6 texture files are named: xp, xn, yp, yn, zp, zn; and all are of type JPG
	std::string xp = mapDir;
	xp += "/xp.jpg";
	std::string xn = mapDir;
	xn += "/xn.jpg";
	std::string yp = mapDir;
	yp += "/yp.jpg";
	std::string yn = mapDir;
	yn += "/yn.jpg";
	std::string zp = mapDir;
	zp += "/zp.jpg";
	std::string zn = mapDir;
	zn += "/zn.jpg";

	textureRef = SOIL_load_OGL_cubemap(xp.c_str(), xn.c_str(), yp.c_str(), yn.c_str(), zp.c_str(), zn.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (textureRef == 0)
		std::cout << "error: Could not find Cube Map image file" << std::endl;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureRef);

	// Reduce the seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureRef;
}

// Convert a float from degrees to radians (and return a float)
float toRadians(float deg) {
	return ( deg * 3.14159f / 180.0f );
}

// Gold Material (Ambient, then Diffuse, Specular, then Shininess)
// Numbers define RGBA values for each
float * goldAmbient() {
	static float a[4] = { 0.2473f, 0.1995f, 0.0745f, 1 };
	return (float *)a;
}

float * goldDiffuse() {
	static float a[4] = { 0.7516f, 0.6065f, 0.2265f, 1 };
	return (float *)a;
}

float * goldSpecular() {
	static float a[4] = { 0.6283f, 0.5559f, 0.5075f, 1 };
	return (float *)a;
}

float goldShininess() {
	return 51.2f;
}

// BRONZE material - ambient, diffuse, specular, and shininess
float* bronzeAmbient() { 
	static float a[4] = { 0.2125f, 0.1275f, 0.0540f, 1 }; return (float*)a; 
}

float* bronzeDiffuse() { 
	static float a[4] = { 0.7140f, 0.4284f, 0.1814f, 1 }; return (float*)a; 
}

float* bronzeSpecular() { 
	static float a[4] = { 0.3936f, 0.2719f, 0.1667f, 1}; return (float*)a; 
}

float bronzeShininess() { 
	return 25.6f; 
}