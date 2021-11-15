/**
 * Name: Pranav Rao
 * Class: CSCI 168 Graphics Programming in OpenGL
 * Description: This program draws an airport scene. You can move around using the mouse and WASD. Toggle the flashlight with F.
 *
 * NOTES:
 * Please check file paths in lines 25-28 and the init() function if linking errors happen
 * SOF (Start) and EOF (End) strings are used in shader files so that multiple shaders can be stored in one file
 * 
 * All models and textures are my original work
 * Toyota, HiAce, and Dassault Falcon are registered trademarks of their respective entities
 * The concrete and grass textures are sourced from Poliigon.com and are used under license
 * "Sobrato Municipal Airfield" is a fictional place and may or may not have anything to do with the Sobrato family
 * 
 */
#include <stack>
#include "Utils_PR.h"
#include "ImportedModel.h"

// Number of Vertex Array Objects and Vertex Buffer Objects (P, T, N for Falcon; P, T for plane; P only for skybox)
#define numVAOs 1
#define numVBOs 25

// Vertex and Fragment shader file paths
const char * vShaderFile = "res/shaders/vertShader_F.glsl";
const char * fShaderFile = "res/shaders/fragShader_F.glsl";
const char * vShaderSkyFile = "res/shaders/vertCShader_F.glsl";
const char * fShaderSkyFile = "res/shaders/fragCShader_F.glsl";

// Texture file paths
const char * skyBoxPath = "res/cubeMap";
const char texPaths[7][50] = {
	"res/textures/ground_plane_tex.png",
	"res/textures/Fuselage_TOTAL_TEX.png",
	"res/textures/hi_ace_tex.png",
	"res/textures/terminal_tex.png",
	"res/textures/tower_tex.png",
	"res/textures/tower_roof_tex.png",
	"res/textures/dish_tex.png"
};

// OBJ file paths
const char meshPaths[7][50] = {
	"res/meshes/dassault_falcon2.obj",
	"res/meshes/hi_ace.obj",
	"res/meshes/terminal.obj",
	"res/meshes/tower.obj",
	"res/meshes/tower_windows.obj",
	"res/meshes/tower_roof.obj",
	"res/meshes/dish.obj"
};

// Create instances of the OBJs
std::vector<ImportedModel> objects;

// Location variables for the matrices
float cameraX, cameraY, cameraZ;

// Whether to keep the cursor fixed (used in main)
bool keepFixed = 1;

// Important variables
GLuint renderingProgram, renderingProgramCubeMap;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

// Declare variables used in display function to save resources
std::stack<glm::mat4> mvStack;

GLuint mvLoc, projLoc, vLoc, nLoc;
GLuint skyboxTexture, facLoc;
GLuint tex[3];
GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mAmbLoc, mDiffLoc, mSpecLoc, mShiLoc, dirLoc, cutoffLoc, isOnLoc;
glm::mat4 pMat, vMat, mMat, mvMat, invTrMat;

int vboInd, texInd, objInd;
int width, height;
float aspect;

//Lights
void installLights(glm::mat4 vMatrix);
glm::vec3 currentLightPos, lightPosV, currentLightDir;
float lightPos[3], lightDir[3];
float lightCutoff = toRadians(12.5f);

glm::vec3 initialLightLoc = glm::vec3(5.0f, -2.0f, 2.0f);

float globalAmbient[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
float lightAmbient[4] = { 0.6f, 0.6f, 0.4f, 1.0f };
float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

float* matAmb = goldAmbient();
float* matDif = goldDiffuse();
float* matSpec = goldSpecular();
float matShi = goldShininess();

float isOn = 1.0f;
bool isPressed = 0;

// For animation
float rotation = -toRadians(2.0f);
float rFactor = 0.0f;
float MAX_ROT = toRadians(10.0f);

float zPosFalcon = 150.0f;
float zPosDFFactor = -0.15f;

float altitude = 5.0f;
float aFactor = -0.005f;
float MIN_ALT = 0.3f;

float zPosHiace = 14.0f;
float zPosHiaceFactor = -0.02f;
float MAX_ZPOS_HIACE = -14.0f;

// Useful axes
glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);

// For Mouse movement and WASD capture
glm::vec3 position, direction, right, up;
float horizontalAngle = 3.14f;
float verticalAngle = 0.0f;
float initialFoV = 45.0f;

double xpos, ypos;

float speed = 3.0f; // WASD movement speed
float mouseSpeed = 0.05f;

/*************************************************   End of Variable declarations  ********************************************/

// Set up an instance of an Imported model (.obj), defined in the VBO at positions <offset> through <offset+2>
void setupVerticesObj(ImportedModel model, unsigned int offset) {
	std::vector<glm::vec3> vert = model.getVertices();
	std::vector<glm::vec2> tex = model.getTextureCoords();
	std::vector<glm::vec3> nrm = model.getNormals();

	// pvalues will store the positions of the vertices in the sphere
	std::vector<float> pvalues;
	std::vector<float> tvalues;
	std::vector<float> nvalues;

	int numObjVertices = model.getNumVertices();

	// Populate position, texture, and normal vectors with the objects's acual values
	for (int i = 0; i < numObjVertices; i++) {
		pvalues.push_back((vert[i]).x);
		pvalues.push_back((vert[i]).y);
		pvalues.push_back((vert[i]).z);

		tvalues.push_back((tex[i]).s);
		tvalues.push_back((tex[i]).t);

		nvalues.push_back((nrm[i]).x);
		nvalues.push_back((nrm[i]).y);
		nvalues.push_back((nrm[i]).z);
	}

	// No need to initialize VBOs and VAOs. That was done in setupVerticesSphere()

	// Put the vertices into the VBO at position 0
	// Multiply by 4 because pvalues contains floats, and each float is 4 bytes (quicker than sizeof(float) ).
	glBindBuffer(GL_ARRAY_BUFFER, vbo[offset]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	// Put texture coordinates into VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo[offset+1]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

	// Put normals into VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo[offset+2]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);

}

// Set up an instance of an square plane with side length radius*2, defined in the VBO at positions <offset> through <offset+2>
void setupVerticesPlane(float radius, unsigned int offset) {
	float vertexPositions[18] = {
		-radius,  0.0f, -radius, -radius, 0.0f, radius, radius, 0.0f, radius,
		radius, 0.0f, radius, radius, 0.0f, -radius, -radius,  0.0f, -radius,
	};

	float texCoords[12] = {
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	};

	float normals[12] = {
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};
	
	// Set up and bind the vertex positions to the vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, vbo[offset]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);

	// Put texture coordinates into VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo[offset+1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);

	// Put normals into VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo[offset + 2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

}

// Set up an instance of a sky box
void setupVerticesBox(void) {
	// The vertex positions for a cube with edge length 2 units
	float cubeVertexPositions[108] = {
		-1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f 
	};

	float cubeTextureCoords[72] = {
		1.00f, 0.66f, 1.00f, 0.33f, 0.75f, 0.33f,
		0.75f, 0.33f, 0.75f, 0.66f, 1.00f, 0.66f, 
		0.75f, 0.33f, 0.50f, 0.33f, 0.75f, 0.66f,
		0.50f, 0.33f, 0.50f, 0.66f, 0.75f, 0.66f, 
		0.50f, 0.33f, 0.25f, 0.33f, 0.50f, 0.66f, 
		0.25f, 0.33f, 0.25f, 0.66f, 0.50f, 0.66f, 
		0.25f, 0.33f, 0.00f, 0.33f, 0.25f, 0.66f, 
		0.00f, 0.33f, 0.00f, 0.66f, 0.25f, 0.66f,
		0.25f, 0.33f, 0.50f, 0.33f, 0.50f, 0.00f,
		0.50f, 0.00f, 0.25f, 0.00f, 0.25f, 0.33f, 
		0.25f, 1.00f, 0.50f, 1.00f, 0.50f, 0.66f, 
		0.50f, 0.66f, 0.25f, 0.66f, 0.25f, 1.00f
	};

	// Set up vertex array objects
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);

	// Set up and bind the vertex positions to the vertex buffer object
	glGenBuffers(numVBOs, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexPositions), cubeVertexPositions, GL_STATIC_DRAW);

}

void init(GLFWwindow* window) {
	renderingProgram = createShaderProgram(vShaderFile, fShaderFile);
	renderingProgramCubeMap = createShaderProgram(vShaderSkyFile, fShaderSkyFile);

	// Compute perspective (camera viewing angle) matrix
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
	
	cameraX = 0.0f;
	cameraY = 0.3f;
	cameraZ = 15.0f;

	position = glm::vec3(cameraX, cameraY, cameraZ);

	// Box must happen first!
	setupVerticesBox();
	skyboxTexture = loadCubeMap(skyBoxPath);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	
	// There are 8 models, one defined here and 7 defined in obj files. Initialize them and their textures now
	setupVerticesPlane(28, 1); // 1 because VBO[0] contains the skybox
	for (int i = 0; i < 7; i++) {
		objects.push_back( ImportedModel(meshPaths[i]) );
		setupVerticesObj(objects[i], (3 * i) + 4);
		tex[i] = loadTexture(texPaths[i]);
	}
	
}

void display(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // Dark Grey
	glClear(GL_COLOR_BUFFER_BIT);

	// Reset counters
	vboInd = 0;
	texInd = 0;
	objInd = 0;

	// Capture Mouse position
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset Mouse to center of screen (FPS controls)
	glfwSetCursorPos(window, width / 2, height / 2);

	// If left-shift is held down, sprint. If left-control is held down, move slowly
	// This order prioritizes sprinting over crawling
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		speed = 9.0f;
	} else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		speed = 1.0f;
	} else {
		speed = 3.0f; // Standard speed as defined earlier
	}

	// Calculate pitch and yaw of camera (float value is 1/60 -- 60 bc 60 FPS)
	horizontalAngle += mouseSpeed * 0.0166f * float(width / 2 - xpos);
	verticalAngle += mouseSpeed * 0.0166f * float(height / 2 - ypos);

	// Calculate forward direction vector
	direction = glm::vec3(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	// Calculate rightward direction vector
	right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f),
		0.0f,
		cos(horizontalAngle - 3.14f/2.0f)
	);

	// Use cross product to ensure up vector really is up (and is affected by rotation)
	up = glm::cross(right, direction);

	// WASD controls. Multiply corresponding vector with time delta and movement speed, and add/subtract result to/from current position
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position += direction * 0.0166f * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position -= right * 0.0166f * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position -= direction * 0.0166f * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += right * 0.0166f * speed;
	}

	// Turn light on or off (on by default)
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !isPressed) {
		isOn *= -1.0f;
		isPressed = 1;
	}
	else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE && isPressed) {
		isPressed = 0;
	}
	glUniform1f(isOnLoc, isOn);

	/*************************************************   Draw the Skybox  ********************************************/
	glUseProgram(renderingProgramCubeMap);

	// Get pointers to the view and projection matrices
	vLoc = glGetUniformLocation(renderingProgramCubeMap, "v_matrix");
	projLoc = glGetUniformLocation(renderingProgramCubeMap, "proj_matrix");

	// Build model matrix for the Skybox and position it at camera
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(cameraX, cameraY, cameraZ));

	// Compute view (camera position) matrix
	vMat = glm::lookAt(position, position + direction, up);
	vMat = glm::rotate(vMat, toRadians(120.0f), yAxis);
	mvStack.push(vMat); // View matrix is at top of stack

	// Bind the model-view matrix and projection matrix with the appropriate matrices in the vertex shader
	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	// Bind the cube to the vertex buffer (makes sure we draw the cube)
	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);

	// Enable vertex attributes (position)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// Activate default skybox texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

	// Enable backface culling 
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	// Set up 3D view
	glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Draw the box
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glEnable(GL_DEPTH_TEST);

	/*************************************************   Draw the Scene   **********************************************/
	glUseProgram(renderingProgram);

	// Get locations for uniform variables
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram, "nrm_matrix");
	facLoc = glGetUniformLocation(renderingProgram, "fac");
	isOnLoc = glGetUniformLocation(renderingProgram, "isOn");

	//std::cout << position.x << ", " << position.y << ", " << position.z << std::endl;
	//std::cout << direction.x << ", " << direction.y << ", " << direction.z << std::endl;

	currentLightPos = glm::vec3(position.x, position.y, position.z);
	currentLightDir = glm::vec3((position + direction).x, (position + direction).y, (position + direction).z);
	//std::cout << currentLightPos.x << ", " << currentLightPos.y << ", " << currentLightPos.z << std::endl;
	installLights(mvStack.top()); // View matrix is at top of stack

	/*************************************************   Ground   **********************************************/

	// Build model-view matrix
	mMat = glm::translate(glm::mat4(1.0f), origin);
	
	mvStack.push(vMat);			// View matrix is at top of stack
	mvStack.top() *= mMat;		// MV matrix for plane is at top of stack
	invTrMat = glm::transpose(glm::inverse(mvStack.top()));

	// Bind new values of uniform variables to their counterparts in the vertex/fragment shader
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glUniform1f(facLoc, 0.95f);

	// Set vertex attributes (Position, Tex, NRM)
	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// Set textures
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex[texInd++]);

	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	mvStack.pop(); // Pop MV matrix -- View matrix is at top of stack

	/***************************************************    Falcon   **********************************************/
	// Animate the z-rotation
	rotation += rFactor;
	if (abs(rotation) >= MAX_ROT) {
		rFactor = 0.0f;
	}

	// Animate z-location
	zPosFalcon += zPosDFFactor;

	// Animate the y-location
	altitude += aFactor;
	if (altitude <= MIN_ALT) {
		aFactor = 0.015f;
		altitude += aFactor;
		rFactor = -toRadians(0.1f);
	}

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, altitude, zPosFalcon));
	mMat = glm::rotate(mMat, toRadians(180.0f), yAxis);
	mMat = glm::rotate(mMat, rotation,			xAxis);
	
	mvStack.push(vMat);	// View matrix copy is at top of stack
	mvStack.top() *= mMat;		// MV matrix for obj is at top of stack
	invTrMat = glm::transpose(glm::inverse(mvStack.top()));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glUniform1f(facLoc, 0.8f);

	// Set vertex attributes (Position, Tex, NRM)
	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex[texInd++]);
	
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, objects[objInd++].getNumVertices());
	mvStack.pop(); // Pop MV matrix -- View matrix is at top of stack

	/***************************************************    Toyota HiAce   **********************************************/
	// Animate z-location of HiAce
	if ((float)currentTime >= 20.0f) {
		zPosHiace += zPosHiaceFactor;
	}

	if (zPosHiace <= MAX_ZPOS_HIACE) {
		zPosHiaceFactor = 0.0f;
	}

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.19f, zPosHiace));
	mMat = glm::scale(mMat, glm::vec3(1.2f, 1.2f, 1.2f));

	mvStack.push(vMat);	// View matrix copy is at top of stack
	mvStack.top() *= mMat;		// MV matrix for obj is at top of stack
	invTrMat = glm::transpose(glm::inverse(mvStack.top()));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glUniform1f(facLoc, 0.8f);

	// Set vertex attributes (Position, Tex, NRM)
	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex[texInd++]);

	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, objects[objInd++].getNumVertices());
	mvStack.pop(); // Pop MV matrix -- View matrix is at top of stack

	/***************************************************    Terminal Building (Parent)  **********************************************/
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.0f, 0.2f));
	mvStack.push(vMat);	// View matrix copy is at top of stack
	mvStack.top() *= mMat;		// MV matrix for obj is at top of stack
	invTrMat = glm::transpose(glm::inverse(mvStack.top()));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glUniform1f(facLoc, 0.9f);

	// Set vertex attributes (Position, Tex, NRM)
	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex[texInd++]);

	glDrawArrays(GL_TRIANGLES, 0, objects[objInd++].getNumVertices());
	// Do not pop MV matrix so that terminal can act as parent for tower and dish

	/***************************************************    Control Tower  **********************************************/
	mMat = glm::translate(glm::mat4(1.0f), origin);
	mvStack.push(mvStack.top());	// MV matrix copy is at top of stack
	mvStack.top() *= mMat;		// MV matrix for obj is at top of stack
	invTrMat = glm::transpose(glm::inverse(mvStack.top()));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glUniform1f(facLoc, 0.9f);

	// Set vertex attributes (Position, Tex, NRM)
	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex[texInd++]);

	glDrawArrays(GL_TRIANGLES, 0, objects[objInd++].getNumVertices());
	mvStack.pop(); // Pop MV matrix -- MV matrix is at top of stack
	
	/***************************************************    Tower window   **********************************************/
	mMat = glm::translate(glm::mat4(1.0f), origin);
	mvStack.push(mvStack.top());	// View matrix copy is at top of stack
	mvStack.top() *= mMat;		// MV matrix for obj is at top of stack
	invTrMat = glm::transpose(glm::inverse(mvStack.top()));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glUniform1f(facLoc, 0.0f);

	// Set vertex attributes (Position, Tex, NRM)
	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// Do not increment texInd because this object has no texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex[texInd]);

	glDrawArrays(GL_TRIANGLES, 0, objects[objInd++].getNumVertices());
	mvStack.pop(); // Pop MV matrix -- MV matrix is at top of stack

	/***************************************************    Tower roof   **********************************************/
	mMat = glm::translate(glm::mat4(1.0f), origin);
	mvStack.push(mvStack.top());	// View matrix copy is at top of stack
	mvStack.top() *= mMat;		// MV matrix for obj is at top of stack
	invTrMat = glm::transpose(glm::inverse(mvStack.top()));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glUniform1f(facLoc, 0.9f);

	// Set vertex attributes (Position, Tex, NRM)
	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex[texInd++]);

	glDrawArrays(GL_TRIANGLES, 0, objects[objInd++].getNumVertices());
	mvStack.pop(); // Pop MV matrix -- MV matrix is at top of stack
	
	/***************************************************    Dish   **********************************************/
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-12.85f, 3.51f, 1.51f));
	mMat = glm::rotate(mMat, (float)currentTime*2, yAxis);

	mvStack.push(mvStack.top());	// View matrix copy is at top of stack
	mvStack.top() *= mMat;		// MV matrix for obj is at top of stack
	invTrMat = glm::transpose(glm::inverse(mvStack.top()));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glUniform1f(facLoc, 0.9f);

	// Set vertex attributes (Position, Tex, NRM)
	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboInd++]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex[texInd++]);

	glDrawArrays(GL_TRIANGLES, 0, objects[objInd++].getNumVertices());
	mvStack.pop(); // Pop MV matrix -- MV matrix is at top of stack

	/***************************************************    Finishing Up   **********************************************/

	// Pop MV matrix -- View matrix is at top of stack
	mvStack.pop();
	// Pop view matrix and we are done
	mvStack.pop(); 
}

void installLights(glm::mat4 vMatrix) {
	// convert light's position to view space and save it in a float array (Results in poor setup)
	//lightPosV = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
	lightPos[0] = currentLightPos.x;
	lightPos[1] = currentLightPos.y;
	lightPos[2] = currentLightPos.z;

	lightDir[0] = currentLightDir.x;
	lightDir[1] = currentLightDir.y;
	lightDir[2] = currentLightDir.z;

	// Get locations of light and material fields in shader
	globalAmbLoc = glGetUniformLocation(renderingProgram, "globalAmbient");
	ambLoc = glGetUniformLocation(renderingProgram, "light.ambient");
	diffLoc = glGetUniformLocation(renderingProgram, "light.diffuse");
	specLoc = glGetUniformLocation(renderingProgram, "ligh.specular");
	posLoc = glGetUniformLocation(renderingProgram, "light.position");

	mAmbLoc = glGetUniformLocation(renderingProgram, "material.ambient");
	mDiffLoc = glGetUniformLocation(renderingProgram, "material.diffuse");
	mSpecLoc = glGetUniformLocation(renderingProgram, "material.specular");
	mShiLoc = glGetUniformLocation(renderingProgram, "material.shininess");

	// Set the uniform light and material values in shader
	glProgramUniform4fv(renderingProgram, globalAmbLoc, 1, globalAmbient);
	glProgramUniform4fv(renderingProgram, ambLoc, 1, lightAmbient);
	glProgramUniform4fv(renderingProgram, diffLoc, 1, lightDiffuse);
	glProgramUniform4fv(renderingProgram, specLoc, 1, lightSpecular);

	glProgramUniform3fv(renderingProgram, posLoc, 1, lightPos);
	glProgramUniform3fv(renderingProgram, dirLoc, 1, lightDir);
	glProgramUniform1f(renderingProgram, cutoffLoc, lightCutoff);

	glProgramUniform4fv(renderingProgram, mAmbLoc, 1, matAmb);
	glProgramUniform4fv(renderingProgram, mDiffLoc, 1, matDif);
	glProgramUniform4fv(renderingProgram, mSpecLoc, 1, matSpec);

	glProgramUniform1f(renderingProgram, mShiLoc, matShi);

}

// Reset perspective matrix when window is resized
void window_resize_callback(GLFWwindow* win, int newWidth, int newHeight) {
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	width = newWidth;
	height = newHeight;

}

int main(void) {
	std::cout << "Hello" << std::endl;
	// Initialize GLFW
	if (!glfwInit())
		exit(EXIT_FAILURE);

	// Verify OpenGL version 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Create window
	GLFWwindow* window = glfwCreateWindow(600, 600, "Pranav Rao - Final Project", NULL, NULL);
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK)
		exit(EXIT_FAILURE);

	// Enable Vertical Synchronization (a.k.a. vsync, prevents screen tearing)
	glfwSwapInterval(1);

	// Reset perspective when user resizes window
	glfwSetWindowSizeCallback(window, window_resize_callback);

	// Hide cursor by default
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	init(window);

	// Main event loop
	while (!glfwWindowShouldClose(window)) {
		// While user is ok with it, run display function
		if(keepFixed)
			display(window, glfwGetTime());
		
		glfwSwapBuffers(window);		// "Paint" the window, ensure vsync is active
		glfwPollEvents();				// Log user input, keystrokes, etc.
		
		// If user hits "Escape," they want to leave the window, so stop running display and show the cursor
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			keepFixed = 0;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		// If user clicks on window again, they want to run the animation, so run display and hide the cursor
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			keepFixed = 1;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
	}

	// Close window
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
