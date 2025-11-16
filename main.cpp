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

using namespace std;
using namespace glm;

//void drawCircle(GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLint noOfSides);

// Global variables
GLFWwindow* window;
const int width = 1024, height = 1024;
float scaleX = 1.5f, scaleY = 0.5f, scaleZ = 0;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool weaponPicked = false;
bool enemiesSpawned = false;
const vec3 weaponStartPos = vec3(-0.85f, 0.85f, 0);
const vec3 weaponOffset = vec3(0.2f, 0.0f, 0);
const float pickDistance = 0.25f;
const float hitDistance = 0.5f;
float weaponAngle = 0.0f;
const float weaponSwing = -60.0f;
const float swingSpeed = 200.0f;
bool isSwinging = false;
bool isSwingingDown = false;
// Enemy structure in order to simplify the management
struct Enemy{
	vec3 position;
	bool existing = false;
	vec4 color;
	float health = 1.0f;
}enemies[2];

Enemy marian;

// Callback function for cursor position
/*void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	std::cout << "The mouse cursor is: " << xpos << " " << ypos << std::endl;

	// TODO Ex 8
	// Don't forget that OpenGL coordinates are between -1,-1 and 1,1
}*/


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && weaponPicked) {
		isSwinging = true;
		isSwingingDown = true;
		for (int i = 0; i < 2; i++) {
			if (enemies[i].existing) {
				float distance = length(enemies[i].position - marian.position);
				if (hitDistance < pickDistance) {
					enemies[i].health -= 0.5f;
					cout << "Enemy " << i << " hit! Health: " << enemies[i].health * 100.0f << endl;
					if (enemies[i].health <= 0.0f) {
						enemies[i].existing = false;
						cout << "Enemy " << i << " defeated!" << endl;
					}
				}
			}
		}
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	float move = 3.0f * deltaTime;
	if(action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_W:
			marian.position.y += move;
			break;
		case GLFW_KEY_S:
			marian.position.y -= move;
			break;
		case GLFW_KEY_A:
			marian.position.x -= move;
			break;
		case GLFW_KEY_D:
			marian.position.x += move;
			break;
		case GLFW_KEY_E:
			if (!weaponPicked) {
				//glm::vec3 marianPos = glm::vec3(marian.position.x, marian.position.y, marian.position.z);
				float distance = length(weaponStartPos - marian.position);
				if(distance < pickDistance) {
					weaponPicked = true;
					cout << "Weapon picked!" << endl;
				}
			}
			break;
		default:
			break;
		}
	}
}

// TODO Ex 4
// Complete callback for adjusting the viewport when resizing the window
void window_callback(GLFWwindow* window, int new_width, int new_height)
{
	glViewport(0, 0, new_width, new_height);
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
	glClearColor(0.18f, 0.7f, 0.95f, 0.0f);
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
	mat4 trans(1.0f);

	// Maybe we can play with different positions
	vec3 positions[] = {
		vec3(0.43f,  -0.03f,  0),//right arm
		vec3(-0.43f,  -0.03f, 0),//left arm
		vec3(0.15f, -0.13f, 0),//right leg
		vec3(-0.15f, -0.13f, 0)//left leg
		//weapon
	};

	

	// Set a callback for handling mouse cursor position
	// Decomment the following line for a cursor position change callback example
	// glfwSetCursorPosCallback(window, cursor_position_callback);

	// TODO Ex4
	// Set callback for window resizing
	glfwSetFramebufferSizeCallback(window, window_callback);

	marian.position = vec3(0.0f, 0.0f, 0.0f);
	marian.existing = true;
	marian.color = vec4(0.0f, 1.0f, 0.0f, 1.0f);

	// Check if the window was closed
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
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

		if (marian.existing) {
			mat4 model = mat4(1.0f);
			mat4 translation_matrix = translate(mat4(1.0f), marian.position);

			glBindVertexArray(circle_vao);
			//drawing the head
			model = scale(model, vec3(0.25f, 0.25f, 1.0f));
			model = translate(model, vec3(0.0f, 0.76f, 0.0f));
			model = translation_matrix * model;
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(model));
			vec4 head_color = vec4(1.0f, 0.8f, 0.6f, 1.0f);
			glUniform4fv(transformLoc2, 1, value_ptr(head_color));
			glDrawArrays(GL_TRIANGLE_FAN, 0, noOfVertices);

			//drawing the body
			model = mat4(1.0f);
			model = scale(model, vec3(0.4f, 0.4f, 1.0f));
			model = translation_matrix * model;
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(model));
			vec4 body_color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
			glUniform4fv(transformLoc2, 1, value_ptr(body_color));
			glDrawArrays(GL_TRIANGLE_FAN, 0, noOfVertices);

			glBindVertexArray(vao);
			//drawing the arms and legs
			vec3 arm_scale = vec3(0.3f, 2.0f, 1.0f);
			vec3 leg_scale = vec3(0.4f, 1.3f, 1.0f);
			vec4 leg_color = vec4(0.56f, 0.63f, 0.84f, 1.0);
			vec3 scaleParts;
			GLfloat angle = 20.0f;
			for (int i = 0; i < 4; i++) {
				if (i >= 2) {
					glUniform4fv(transformLoc2, 1, value_ptr(leg_color));
					scaleParts = leg_scale;
				}
				else {
					glUniform4fv(transformLoc2, 1, value_ptr(body_color));
					scaleParts = arm_scale;
				}
				model = mat4(1.0f);

				if (i == 0) {
					model = rotate(model, angle, vec3(0.0f, 0.0f, 1.0f));
				}
				if (i == 1) {
					model = rotate(model, -angle, vec3(0.0f, 0.0f, 1.0f));
				}
				model = scale(model, scaleParts);
				model = translate(model, positions[i]);
				model = translation_matrix * model;

				glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(model));
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
		}
		// Initializing the enemies after the weapon is picked
		if (weaponPicked && !enemiesSpawned) {
			enemies[0].position = vec3(-0.7f, -0.7f, 0.0f);//botom left
			enemies[0].color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
			enemies[0].existing = true;
			enemies[1].position = vec3(0.5f, 0.1f, 0.0f);//middle right
			enemies[1].color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
			enemies[1].existing = true;

			enemiesSpawned = true;
			cout << "Enemies spawned!" << '\n';
		}

		for (int j = 0; j < 2; j++) {
			if (enemies[j].existing) {
				mat4 model = mat4(1.0f);
				mat4 translation_matrix = translate(mat4(1.0f), enemies[j].position);

				glBindVertexArray(circle_vao);
				//drawing the head
				model = scale(model, vec3(0.25f, 0.25f, 1.0f));
				model = translate(model, vec3(0.0f, 0.76f, 0.0f));
				model = translation_matrix * model;
				glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(model));
				vec4 head_color = vec4(1.0f, 0.8f, 0.6f, 1.0f);
				glUniform4fv(transformLoc2, 1, value_ptr(head_color));
				glDrawArrays(GL_TRIANGLE_FAN, 0, noOfVertices);

				//drawing the body
				model = mat4(1.0f);
				model = scale(model, vec3(0.4f, 0.4f, 1.0f));
				model = translation_matrix * model;
				glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(model));
				vec4 body_color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
				glUniform4fv(transformLoc2, 1, value_ptr(body_color));
				glDrawArrays(GL_TRIANGLE_FAN, 0, noOfVertices);

				glBindVertexArray(vao);
				//drawing the arms and legs
				vec3 arm_scale = vec3(0.3f, 2.0f, 1.0f);
				vec3 leg_scale = vec3(0.4f, 1.3f, 1.0f);
				vec4 leg_color = vec4(0.56f, 0.63f, 0.84f, 1.0);
				vec3 scaleParts;
				GLfloat angle = 20.0f;
				for (int i = 0; i < 4; i++) {
					if (i >= 2) {
						glUniform4fv(transformLoc2, 1, value_ptr(leg_color));
						scaleParts = leg_scale;
					}
					else {
						glUniform4fv(transformLoc2, 1, value_ptr(body_color));
						scaleParts = arm_scale;
					}
					model = mat4(1.0f);

					if (i == 0) {
						model = rotate(model, angle, vec3(0.0f, 0.0f, 1.0f));
					}
					if (i == 1) {
						model = rotate(model, -angle, vec3(0.0f, 0.0f, 1.0f));
					}
					model = scale(model, scaleParts);
					model = translate(model, positions[i]);
					model = translation_matrix * model;

					glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(model));
					glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				}
			}
		}
		//Swinging the weapon
		if (isSwinging) {
			if (isSwingingDown) {
				weaponAngle -= swingSpeed * deltaTime;
				if(weaponAngle <= weaponSwing) {
					weaponAngle = weaponSwing;
					isSwingingDown = false;
				}
			}
			else {
				weaponAngle += swingSpeed * deltaTime;
				if (weaponAngle >= 0.0f) {
					weaponAngle = 0.0f;
					isSwinging = false;
				}
			}
		}

		//When swinging the weapon, i used radians(weaponAngle). but the movement was very subtle
		//and out of curiosity i tried to use just weaponAngle and seemed to work better 
		//but i still can't figure how to rotate wrt the arm not on its own

		// Drawing the weapon for Marian
		glUniform4fv(transformLoc2, 1, value_ptr(vec4(0.82f,0.82f,0.82f,1.0f)));
		glBindVertexArray(vao);
		mat4 model = mat4(1.0f);
		if (!weaponPicked) {
			model = translate(model, weaponStartPos);
			model = rotate(model, weaponAngle, vec3(0.0f, 0.0f, 1.0f));
		}
		else {
			mat4 translation_matrix = translate(mat4(1.0f), marian.position);
			model = translate(model, weaponOffset);
			model = translation_matrix * model;
			model = rotate(model, weaponAngle, vec3(0.0f, 0.0f, 1.0f));
		}

		model = scale(model, vec3(0.3f, 2.0f, 1.0f));

		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(model));

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Drawing health bar above Marian and above enemies

		if (weaponPicked) {
			glBindVertexArray(vao);
			float currentHealth = marian.health;
			//simulate the health loss
			//float lostHealth = 0.5f;
			float lostHealth = 1.0f - currentHealth;
			vec3 healthBarPos = marian.position + vec3(0.0f, 0.3f, 0.0f);
			float barWidth = 0.3f;
			float barHeight = 0.02f;

			model = mat4(1.0f);
			model = translate(model, healthBarPos);
			model = scale(model, vec3(barWidth/0.1f, barHeight/0.1f, 1.0f));
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(model));
			glUniform4fv(transformLoc2, 1, value_ptr(vec4(0.0f, 1.0f, 0.0f, 1.0f)));
			if (lostHealth < 1.0f)
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			if(lostHealth > 0.0f){
				//calculating the red part of the health bar
				float redHealth = barWidth * lostHealth;
				float offsetX = ( barWidth - redHealth) / 2.0f ;
				model = mat4(1.0f);
				model = translate(model, healthBarPos + vec3(offsetX, 0.0f, 0.0f));
				model = scale(model, vec3(redHealth/0.1f, barHeight/0.1f, 1.0f));
				glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(model));
				glUniform4fv(transformLoc2, 1, value_ptr(vec4(1.0f, 0.0f, 0.0f, 1.0f)));
				if(lostHealth < 1.0f)
					glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}


			for (int j = 0; j < 2; j++) {
				glBindVertexArray(vao);
				float currentHealth = enemies[j].health;
				float lostHealth = 1.0f - currentHealth;
				vec3 healthBarPos = enemies[j].position + vec3(0.0f, 0.3f, 0.0f);

				model = mat4(1.0f);
				model = translate(model, healthBarPos);
				model = scale(model, vec3(barWidth/0.1f, barHeight/0.1f, 1.0f));
				glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(model));
				glUniform4fv(transformLoc2, 1, value_ptr(vec4(0.0f, 1.0f, 0.0f, 1.0f)));
				if (lostHealth < 1.0f)
					glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

				if(lostHealth > 0.0f){
					//calculating the red part of the health bar
					float redHealth = barWidth * lostHealth;
					float offsetX = (barWidth - redHealth) /1.5f ;
					model = mat4(1.0f);
					model = translate(model, healthBarPos + vec3(offsetX, 0.0f, 0.0f));
					model = scale(model, vec3(barWidth / 0.1f, barHeight / 0.1f, 1.0f));
					glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(model));
					glUniform4fv(transformLoc2, 1, value_ptr(vec4(1.0f, 0.0f, 0.0f, 1.0f)));
					if (lostHealth < 1.0f)
						glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				}
			}
		}

		glfwSetKeyCallback(window, key_callback);
		glfwSetMouseButtonCallback(window, mouse_button_callback);
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