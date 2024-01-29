#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glrt/endl.h>

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

GLFWwindow *g_window;
const int g_width = 800, g_height = 600;

GLint g_status;

/* Very basic source code for a vertex shader. */
const char *g_vertex_shader_source = "#version 330 core" ENDL "layout (location = 0) in vec3 aPos;" ENDL "void main()" ENDL "{" ENDL "gl_Position = vec4(aPos, 1.0);" ENDL "}" ENDL;

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

	/* Load OpenGL context to the window, using GLAD2. */
	if(gladLoadGL(glfwGetProcAddress) == 0)
	{
		fprintf(stderr, "Failed to load the OpenGL context to our window! Maybe GLAD is at fault?" ENDL);
		exit(EXIT_FAILURE);
	}

	/* Compile the vertex shader. */
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &g_vertex_shader_source, NULL);
	glCompileShader(vertex_shader);

	/* Open the fragment shader source file for reading. */
	FILE *fragment_shader_file = fopen("fragment.glsl", "rb"); // Open in binary mode
	if (!fragment_shader_file)
	{
		fprintf(stderr, "Failed to open the fragment shader source file for reading!\n");
		exit(EXIT_FAILURE);
	}

	/* Get the size of the fragment shader source file. */
	fseek(fragment_shader_file, 0, SEEK_END);
	long fragment_shader_source_size = ftell(fragment_shader_file);
	rewind(fragment_shader_file);

	/* Allocate memory to store the fragment shader source code. */
	char *fragment_shader_source = (char *)malloc(fragment_shader_source_size + 1);

	/*
	 *	Read the fragment shader source code from the file, and add a
	 *	null-terminator at the end.
	 */
	size_t bytesRead = fread(fragment_shader_source, 1, fragment_shader_source_size, fragment_shader_file);
	if (bytesRead != fragment_shader_source_size)
	{
		fprintf(stderr, "Failed to read the fragment shader source file!\n");
		exit(EXIT_FAILURE);
	}
	fragment_shader_source[fragment_shader_source_size] = '\0';

	/* Close the fragment shader source file. */
	fclose(fragment_shader_file);

	/* Compile the fragment shader. */
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);

	/* Check for fragment shader compilation errors. */
	GLint fragment_status;
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_status);
	if(!fragment_status)
	{
		GLint log_size;
		glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_size);
		char *log_message = (char *)malloc(log_size * sizeof(char));
		glGetShaderInfoLog(fragment_shader, log_size, NULL, log_message);
		fprintf(stderr, "Failed to compile fragment shader:" ENDL "%s" ENDL, log_message);
		free(log_message);
		exit(EXIT_FAILURE);
	}

	/* De-allocate the fragment shader source code, as it's now compiled. */
	free(fragment_shader_source);

	/* Link the vertex & fragment shaders together into a shader program. */
	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	/* Check if link succeeded or failed. */
	GLint status;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &status);
	if(!status)
	{
		GLint log_size;
		glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &log_size);
		char *log_message = (char*)malloc(log_size * sizeof(char));
		glGetProgramInfoLog(shader_program, log_size, NULL, log_message);
		fprintf(stderr, "Failed to link the shader program:" ENDL "%s" ENDL, log_message);
		free(log_message);
		exit(EXIT_FAILURE);
	}

	/* Delete the shaders, as they're now linked. */
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	/* The vertices and indices of the screen-filling quad. */
	float vertices[] = {1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f};
	GLuint indices[] = {0, 1, 3, 1, 2, 3};

	/*
	 *	Create vertex-array-object, vertex-buffer-object, and
	 *	element-buffer-object.
	 *	And then of course, initialize them.
	 */
	GLuint vbo, vao, ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	/* The loop that persists the rendering! */
	while(!glfwWindowShouldClose(g_window))
	{
		/* Clear the window. */
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		/* Select the shader to be used, and draw the screen-filling quad. */
		glUseProgram(shader_program);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		/* Swap buffers & poll events. */
		glfwSwapBuffers(g_window);
		glfwPollEvents();
	}

	/*
	 *	Clean-up the vertex-array-object, vertex-buffer-object, and\
	 *	element-buffer-object.
	 */
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteProgram(shader_program);

	exit(EXIT_SUCCESS);
}