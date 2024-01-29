#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glrt/endl.h>

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

GLFWwindow *g_window;
const int g_width = 800, g_height = 600;

int main()
{
	/*
	 *	Initialize GLFW. This must be done before any other GLFW functions are
	 *	called. If GLFW fails to initialize, it will print an error & exit the
	 *	program.
	 */
	if(!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW!" ENDL);
		exit(EXIT_FAILURE);
	}

	/*
	 *	Register the GLFW terminate function as a callback that gets called on
	 *	exit.
	 */
	atexit(glfwTerminate);

	/*
	 *	Create the window using GLFW. Here, we use the default width & height.
	 *	We print an error & exit if GLFW fails to correctly create the window.
	 */
	g_window = glfwCreateWindow(g_width, g_height, "glrt", NULL, NULL);
	if(!g_window)
	{
		fprintf(stderr, "Failed to create a window! Maybe GLFW is at fault?" ENDL);
		exit(EXIT_FAILURE);
	}

	/* Select the window as the current window to work on. */
	glfwMakeContextCurrent(g_window);

	if(gladLoadGL(glfwGetProcAddress) == 0)
	{
		fprintf(stderr, "Failed to load the OpenGL context to our window! Maybe GLAD is at fault?" ENDL);
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}