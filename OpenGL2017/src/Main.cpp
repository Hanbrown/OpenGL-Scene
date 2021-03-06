////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Contains main function to create a window and run engine that
///        repeatedly generates a framebuffer and displays it.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes

// STL
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

// Engine
#include "GLInclude.h"

////////////////////////////////////////////////////////////////////////////////
// Global variables - avoid these

// Window
int g_width{ 1360 };
int g_height{ 768 };

// Framebuffer
std::unique_ptr<glm::vec4[]> g_frame{ nullptr }; ///< Framebuffer

// Frame rate
const unsigned int FPS = 60;
float g_frameRate{ 0.f };
std::chrono::high_resolution_clock::time_point g_frameTime{
  std::chrono::high_resolution_clock::now() };
float g_delay{ 0.f };
float g_framesPerSecond{ 0.f };

////////////////////////////////////////////////////////////////////////////////
// Functions

////////////////////////////////////////////////////////////////////////////////
/// @brief Initialize GL settings
void
initialize(GLFWwindow* _window) {
	glClearColor(0.f, 0.f, 0.f, 1.f);

	g_frame = std::make_unique<glm::vec4[]>(g_width*g_height);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Callback for resize of window
///
/// Responsible for setting window size (viewport) and projection matrix.
void resize(GLFWwindow* window, int _w, int _h) {
	g_width = _w;
	g_height = _h;

	// Viewport
	glfwGetFramebufferSize(window, &g_width, &g_height);
	glViewport(0, 0, g_width, g_height);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Draw function for single frame
void
draw(GLFWwindow* _window, double _currentTime) {
	//////////////////////////////////////////////////////////////////////////////
	// Clear
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < g_width*g_height; ++i)
		g_frame[i] = glm::vec4(0.f, 0.4f, 0.f, 0.f);

	//////////////////////////////////////////////////////////////////////////////
	// Draw

	// Simple static :P
	for (int i = 0; i < g_width*g_height; ++i)
		g_frame[i] = glm::vec4(float(rand()) / RAND_MAX, float(rand()) / RAND_MAX,
			float(rand()) / RAND_MAX, 1.f);

	glDrawPixels(g_width, g_height, GL_RGBA, GL_FLOAT, g_frame.get());
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Main application loop
void
run(GLFWwindow* _window) {
	using namespace std::chrono;

	std::cout << "Starting main loop" << std::endl;

	while (!glfwWindowShouldClose(_window)) {
		draw(_window, glfwGetTime());
		////////////////////////////////////////////////////////////////////////////
		// Show
		glfwSwapBuffers(_window);
		glfwPollEvents();

		////////////////////////////////////////////////////////////////////////////
		// Record frame time
		high_resolution_clock::time_point time = high_resolution_clock::now();
		g_frameRate = duration_cast<duration<float>>(time - g_frameTime).count();
		g_frameTime = time;
		g_framesPerSecond = 1.f / (g_delay + g_frameRate);
		printf("FPS: %6.2f\n", g_framesPerSecond);

		////////////////////////////////////////////////////////////////////////////
		// Delay to fix the frame-rate
		g_delay = std::max(0.f, 1.f / FPS - g_frameRate);
		std::this_thread::sleep_for(std::chrono::microseconds(
			static_cast<long int>(g_delay * 1000)));
	}

	std::cout << "Ending main loop" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Callback function for keyboard input
/// @param _window
/// @param _key Key
/// @param _scancode Platform specific keycode
/// @param _action Action, e.g., pressed or released
/// @param _mods Keyboard modifiers
void
keyCallback(GLFWwindow* _window, int _key, int _scancode,
	int _action, int _mods) {
	if (_action == GLFW_PRESS) {
		switch (_key) {
			// Escape key : quit application
		case GLFW_KEY_ESCAPE:
			std::cout << "Closing window" << std::endl;
			glfwSetWindowShouldClose(_window, GLFW_TRUE);
			break;
			// Arrow keys
		case GLFW_KEY_LEFT:
		case GLFW_KEY_RIGHT:
			break;
			// Unhandled
		default:
			std::cout << "Unhandled key: " << _key << std::endl;
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Error handler
/// @param _code Error code
/// @param _msg Error message
void errorCallback(int _code, const char* _msg) {
	std::cerr << "Error " << _code << ": " << _msg << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
// Main

////////////////////////////////////////////////////////////////////////////////
/// @brief main
/// @param _argc Count of command line arguments
/// @param _argv Command line arguments
/// @return Application success status
int
main(int _argc, char** _argv) {
	//////////////////////////////////////////////////////////////////////////////
	// Initialize
	std::cout << "Initializing GLFWWindow" << std::endl;
	// GLFW
	glfwSetErrorCallback(errorCallback);
	if (!glfwInit()) {
		std::cerr << "GLFW Cannot initialize" << std::endl;
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(
		g_width, g_height, "Spiderling: A Rudamentary Game Engine", NULL, NULL);
	if (!window) {
		std::cerr << "GLFW Cannot create window" << std::endl;
		return 1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	glfwGetFramebufferSize(window, &g_width, &g_height);
	glViewport(0, 0, g_width, g_height);  // Initialize viewport


	// Assign callback functions
	std::cout << "Assigning Callback functions" << std::endl;
	glfwSetFramebufferSizeCallback(window, resize);
	glfwSetKeyCallback(window, keyCallback);

	// Program initialize
	std::cout << "Initializing application" << std::endl;
	initialize(window);

	//////////////////////////////////////////////////////////////////////////////
	// Main loop
	run(window);

	//////////////////////////////////////////////////////////////////////////////
	// Cleanup GLFW Window
	std::cout << "Destroying GLFWWindow" << std::endl;
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}



/*
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

void init(GLFWwindow* window) {}

void display(GLFWwindow* window, double currentTime) {
	glClearColor(0.0, 0.8, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
}

int main(void) {
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	GLFWwindow* window = glfwCreateWindow(600, 600, "Title", NULL, NULL);

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		exit(EXIT_FAILURE);
	}

	glfwSwapInterval(1);

	init(window);

	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);

}
*/