// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>

// Include GLEW and GLFW
#include "dependente\glew\glew.h"
#include "dependente\glfw\glfw3.h"

// Include GLM
#include "dependente\glm\glm.hpp"
#include "dependente\glm\gtc\matrix_transform.hpp"
#include "dependente\glm\gtc\type_ptr.hpp"

// Include our helper function for loading shaders
#include "shader.hpp"

//void drawCircle(GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLint noOfSides);

// Global variables
GLFWwindow* window;
const int width = 1024, height = 1024;
float scaleX = 1.5f, scaleY = 0.5f, scaleZ = 0;

// Callback function for cursor position
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	std::cout << "The mouse cursor is: " << xpos << " " << ypos << std::endl;

	// TODO Ex 8
	// Don't forget that OpenGL coordinates are between -1,-1 and 1,1
}

// TODO Ex 3
// Add callback for mouse button

// TODO Ex 4
// Complete callback for adjusting the viewport when resizing the window
void window_callback(GLFWwindow* window, int new_width, int new_height)
{
	// What should we do here :thinking-emoji:?
}

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		(void)getchar();
		return -1;
	}

	// Open a window and create its OpenGL context, retrieve the value into the global variable
	window = glfwCreateWindow(width, height, "Lab 4", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.");
		(void)getchar();
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		(void)getchar();
		glfwTerminate();
		return -1;
	}

	// Specify the size of the rendering window
	glViewport(0, 0, width, height);

	// Clear framebuffer with dark blue color
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Use our helper function to load the shaders from the specified files
	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

	GLfloat vertices[] = {
		0.05f,  0.05f, 0.0f,	// top right
		0.05f, -0.05f, 0.0f,	// bottom right
		-0.05f, -0.05f, 0.0f,	// bottom left
		-0.05f,  0.05f, 0.0f	// top left 
	};

	GLuint indices[] = {	// Note that we start from 0
		0, 3, 1,			// first Triangle
		1, 3, 2,			// second Triangle
	};

	// Create VAO, VBO and IBO
	GLuint vbo, vao, ibo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	// Bind VAO
	glBindVertexArray(vao);

	// Bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Bind IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Describe the data in the buffer
	// This will be accesible in the shaders
	glVertexAttribPointer(
		0,                  // attribute 0, must match the layout in the shader.
		3,                  // size of each attribute (3 floats in this case)
		GL_FLOAT,           // type of data
		GL_FALSE,           // should be normalized?
		3 * sizeof(float),  // stride for this attribute
		(void*)0            // initial offset for array buffer
	);
	glEnableVertexAttribArray(0);

	const GLfloat circle_x = 0.0f; // Center X
	const GLfloat circle_y = 0.0f; // Center Y
	const GLfloat circle_z = 0.0f; // Center Z
	const GLfloat circle_radius = 0.3f; // Radius
	const GLint noOfSides = 50;

	const GLint noOfVertices = noOfSides + 2; // Center + sides
	GLfloat doublePi = 2.0f * 3.14159265358979323846;

	// Create an array to hold the center (0,0,0) and all side points (3 floats per vertex)
	GLfloat circle_vertices[noOfVertices * 3];

	// Center vertex (0,0,0)
	circle_vertices[0] = circle_x;
	circle_vertices[1] = circle_y;
	circle_vertices[2] = circle_z;

	// Vertices for the sides
	for (int i = 1; i < noOfVertices; i++) {
		GLfloat angle = (GLfloat)(i - 1) * doublePi / noOfSides;
		circle_vertices[i * 3] = circle_x + (circle_radius * cos(angle));
		circle_vertices[(i * 3) + 1] = circle_y + (circle_radius * sin(angle));
		circle_vertices[(i * 3) + 2] = circle_z; // Z is 0
	}

	// Create VAO and VBO for the circle
	GLuint circle_vbo, circle_vao;
	glGenVertexArrays(1, &circle_vao);
	glGenBuffers(1, &circle_vbo);

	// Bind Circle VAO
	glBindVertexArray(circle_vao);

	// Bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, circle_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(circle_vertices), circle_vertices, GL_STATIC_DRAW);

	// Describe the data in the buffer for the circle
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Unbind circle VAO
	glBindVertexArray(0);

	// Create identity matrix for transforms
	glm::mat4 trans(1.0f);

	// Maybe we can play with different positions
	glm::vec3 positions[] = {
		glm::vec3(0.0f,  0.0f,  0),
		glm::vec3(0.2f,  0.5f, 0),
		glm::vec3(-0.15f, -0.22f, 0),
		glm::vec3(-0.38f, -0.2f, 0),
		glm::vec3(0.24f, -0.4f, 0),
		glm::vec3(-0.17f,  0.3f, 0),
		glm::vec3(0.23f, -0.2f, 0),
		glm::vec3(0.15f,  0.2f, 0),
		glm::vec3(0.15f,  0.7f, 0),
		glm::vec3(-0.13f,  0.1f, 0)
	};

	// Set a callback for handling mouse cursor position
	// Decomment the following line for a cursor position change callback example
	// glfwSetCursorPosCallback(window, cursor_position_callback);

	// TODO Ex4
	// Set callback for window resizing
	glfwSetFramebufferSizeCallback(window, window_callback);

	// Check if the window was closed
	while (!glfwWindowShouldClose(window))
	{
		// Swap buffers
		glfwSwapBuffers(window);
		// Check for events
		glfwPollEvents();
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);
		// Use our shader
		glUseProgram(programID);
		unsigned int transformLoc = glGetUniformLocation(programID, "transform");
		unsigned int transformLoc2 = glGetUniformLocation(programID, "color");
		//draw the body
		glm::mat4 model_matrix = glm::mat4(1.0f);
		model_matrix = glm::scale(model_matrix, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));

		glm::vec4 body_color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		glUniform4fv(transformLoc2, 1, glm::value_ptr(body_color));

		// Bind the circle's VAO
		glBindVertexArray(circle_vao);

		// Draw the circle using GL_TRIANGLE_FAN
		// The circle_vertices array is set up for GL_TRIANGLE_FAN: Center (0) -> V1 -> V2 -> ... -> Vn
		glDrawArrays(GL_TRIANGLE_FAN, 0, noOfVertices);
		
		//draw the head
		model_matrix = glm::mat4(1.0f);
		model_matrix = glm::scale(model_matrix, glm::vec3(0.25f, 0.25f, 1.0f));
		model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 0.76f, 0.0f));
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glm::vec4 head_color = glm::vec4(1.0f, 0.8f, 0.6f, 1.0f);
		glUniform4fv(transformLoc2, 1, glm::value_ptr(head_color));
		glDrawArrays(GL_TRIANGLE_FAN, 0, noOfVertices);

		// Apply z-axis rotation to our transform matrix
		//trans = glm::rotate(trans, 0.5f, glm::vec3(0.0, 0.0, 1.0));
		//drawCircle(0.0f, 0.0f, 0.0f, 0.5f, 50);
		// Bind VAO
		glBindVertexArray(vao);

		// Send variables to shaders via uniforms
		
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
		
		
		glm::vec4 color = glm::vec4(0.5f, 0, 0.5f, 1.0);
		glUniform4fv(transformLoc2, 1, glm::value_ptr(color));

		// Draw call
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	// Cleanup
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}


/*void drawCircle(GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLint noOfSides) {
	GLint noOfVertices = noOfSides + 2;
	GLfloat doublePi = 2.0f * M_PI;
	GLfloat circleVerticesX[noOfVertices];
	GLfloat circleVerticesY[noOfVertices];
	GLfloat circleVerticesZ[noOfVertices];
	circleVerticesX[0] = x;
	circleVerticesY[0] = y;
	circleVerticesZ[0] = z;
	for(int i = 1; i < noOfVertices; i++) {
		circleVerticesX[i] = x + (radius * cos(i * doublePi / noOfSides));
		circleVerticesY[i] = y + (radius * sin(i * doublePi / noOfSides));
		circleVerticesZ[i] = z;
	}
	GLfloat allCircleVertices[noOfVertices * 3];
	for (int i = 0; i < noOfVertices; i++) {
		allCircleVertices[i * 3] = circleVerticesX[i];
		allCircleVertices[(i * 3) + 1] = circleVerticesY[i];
		allCircleVertices[(i * 3) + 2] = circleVerticesZ[i];
	}

	glVertexPointer(3, GL_FLOAT, 0, allCircleVertices);
	glDrawArrays(GL_TRIANGLE_FAN, 0, noOfVertices);
}*/