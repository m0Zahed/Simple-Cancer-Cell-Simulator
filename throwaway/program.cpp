#include <cstdlib>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Main entry point
int main()
{
	// Attempt to initialize GLFW
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	// Create a window object
	GLFWwindow* pWindow = glfwCreateWindow(500, 500, "GLFW Based OpenGL Window", nullptr, nullptr);

	// Check if the window was created successfully
	if (!pWindow)
	{
		exit(EXIT_FAILURE);
	}

	// Set the context
	glfwMakeContextCurrent(pWindow);

	// Attempt to initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		exit(EXIT_FAILURE);
	}

	// Create a vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// Set the vertex shader source code
	const GLchar* vertexShaderSource =
		"#version 460\n"
		"void main()"
		"{"
		"	gl_Position = vec4(0.0f, 0.0f, 0.0f, 1.0f);"
		"}";

	// Set and compile the vertex shader source code
	glShaderSource(vertexShader, 1, &vertexShaderSource, 0);
	glCompileShader(vertexShader);

	// Check for compile errors
	int vsSuccess;
	char vsCompileLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vsSuccess);
	if (!vsSuccess)
	{
		glGetShaderInfoLog(vertexShader, 512, nullptr, vsCompileLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILE" << std::endl << vsCompileLog << std::endl;
	}

	// Create a fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Set the fragment shader source code
	const GLchar* fragmentShaderSource =
		"#version 460\n"
		"out vec4 fragColor;"
		"void main()"
		"{"
		"	fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);"
		"}";

	// Set and compile the fragment shader source code
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, 0);
	glCompileShader(fragmentShader);

	// Check for compile errors
	int fsSuccess;
	char fsCompileLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fsSuccess);
	if (!fsSuccess)
	{
		glGetShaderInfoLog(fragmentShader, 512, nullptr, fsCompileLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILE" << std::endl << fsCompileLog << std::endl;
	}

	// Create and link a shader program with the shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Check for link errors
	int linkSuccess;
	char linkCompileLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkSuccess);
	if (!linkSuccess)
	{
		glGetProgramInfoLog(shaderProgram, 512, nullptr, linkCompileLog);
		std::cout << "ERROR::SHADER_PROGRAM::LINKING" << std::endl << linkCompileLog << std::endl;
	}

	float inputs[] = {
		0.5f , 0.5f,
		-0.5f, 0.5f,
		-0.5f, -0.5f,
		0.5f, -0.5f
	};

	unsigned int VBO,VAO;
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &VBO);
	

	// Set the shader program to be used
	glUseProgram(shaderProgram);

	// Set the point size
	glPointSize(45.0f);

	// Begin the draw loop
	while (!glfwWindowShouldClose(pWindow))
	{
		// Clear the framebuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw
		glDrawArrays(GL_POINTS, 0, 1);

		// Swap the buffer
		glfwSwapBuffers(pWindow);

		// Check for input(s)
		glfwPollEvents();
	}

	// Satisfy the main function
	return 0;
}