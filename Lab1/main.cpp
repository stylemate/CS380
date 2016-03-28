// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// Shader library
#include <common/shader.hpp>

#define BUFFER_OFFSET( offset ) ((GLvoid*) (offset))

GLFWwindow* window;
int windowWidth, windowHeight;

GLuint programID;
GLuint VAID;
GLuint VBID;

std::vector<glm::vec3> g_vertex_buffer_data;

glm::mat4 Projection;
glm::mat4 View;
float degree = 0.0f;
float initial = 1.0f;
float position = 1.0f;

//// TODO: Implement koch snowflake
//void koch_line(glm::vec3 a, glm::vec3 b, int iter)
//{
//	printf("a.x: %f a.y: %f iter: %d\n", a.x, a.y, iter);
//	if (iter == 0)
//	{
//		g_vertex_buffer_data.push_back(a);
//		g_vertex_buffer_data.push_back(b);
//		printf("pushed\n");
//	}
//	else
//	{
//		float deltaX = b.x - a.x;
//		float deltaY = b.y - a.y;
//
//		float x2 = a.x + deltaX / 3;
//		float y2 = a.y + deltaY / 3;
//
//		float x3 = 0.5 * (a.x + b.x) + sqrt(3) * ((a.y - b.y) / 6);
//		float y3 = 0.5 * (a.y + b.y) - sqrt(3) * ((b.x - a.x) / 6);
//
//		float x4 = a.x + (2 * deltaX / 3);
//		float y4 = a.y + (2 * deltaY / 3);
//
//		koch_line(a, glm::vec3(x2, y2, 0.0f), iter - 1);
//		koch_line(glm::vec3(x2, y2, 0.0f), glm::vec3(x3, y3, 0.0f), iter - 1);
//		koch_line(glm::vec3(x3, y3, 0.0f), glm::vec3(x4, y4, 0.0f), iter - 1);
//		koch_line(glm::vec3(x4, y4, 0.0f), b, iter - 1);
//		printf("type: %s\n", typeid(deltaX).name());
//	}
//}
//
//	Trial 1: FAIL
//	It seems like glDrawArrays does not like shared or duplicated vertices.
//	glDrawElements seems to be able to process and optimize shared vertices through indexing, but I'll need another algorithm to create all those indices...

// TODO: Implement koch snowflake
void koch_line(glm::vec3 a, glm::vec3 b, int iter)
{
	if (iter == 0)
		return;

	float deltax = b.x - a.x;
	float deltay = b.y - a.y;
	
	float x2 = a.x + deltax / 3.0f;
	float y2 = a.y + deltay / 3.0f;
	
	float x3 = ((a.x + b.x) / 2.0f) - (sqrt(3.0f) / 6.0f * ((b.y - a.y)));
	float y3 = ((a.y + b.y) / 2.0f) + (sqrt(3.0f) / 6.0f * ((b.x - a.x)));
	
	float x4 = a.x + (2.0f * deltax / 3.0f);
	float y4 = a.y + (2.0f * deltay / 3.0f);

	glm::vec3 v1 = glm::vec3(x2, y2, 0.0f);
	glm::vec3 v2 = glm::vec3(x3, y3, 0.0f);
	glm::vec3 v3 = glm::vec3(x4, y4, 0.0f);

	g_vertex_buffer_data.push_back(v1);
	g_vertex_buffer_data.push_back(v2);
	g_vertex_buffer_data.push_back(v3);

	koch_line(a, v1, iter - 1);
	koch_line(v1, v2, iter - 1);
	koch_line(v2, v3, iter - 1);
	koch_line(v3, b, iter - 1);
}

//	Trial 2: SUCCESS
//	There are total 5 vertices that can be processed in koch_line method.
//	It's important NOT to push the initial vertex and destination vertex into the buffer if you want to avoid overlapping vertices.
//	Finally, it's important to check the type of a variable so that it is not altered through out the end. 

// TODO: Initialize model
void init_model(void)
{
	g_vertex_buffer_data = std::vector<glm::vec3>();
	g_vertex_buffer_data.push_back(glm::vec3(-0.5f, -0.25f, 0.0f));
	g_vertex_buffer_data.push_back(glm::vec3(0.0f, sqrt(0.75) - 0.25f, 0.0f));
	g_vertex_buffer_data.push_back(glm::vec3(0.5f, -0.25f, 0.0f));

	koch_line(g_vertex_buffer_data[0], g_vertex_buffer_data[1], 5);
	koch_line(g_vertex_buffer_data[1], g_vertex_buffer_data[2], 5);
	koch_line(g_vertex_buffer_data[2], g_vertex_buffer_data[0], 5);

	printf("VBO_SIZE:%zd ", g_vertex_buffer_data.size());

	// Generates Vertex Array Objects in the GPU¡¯s memory and passes back their identifiers
	// Create a vertex array object that represents vertex attributes stored in a vertex buffer object.
	glGenVertexArrays(1, &VAID);
	glBindVertexArray(VAID);
	// Create and initialize a buffer object, Generates our buffers in the GPU¡¯s memory
	glGenBuffers(1, &VBID);
	glBindBuffer(GL_ARRAY_BUFFER, VBID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*g_vertex_buffer_data.size(),
	&g_vertex_buffer_data[0], GL_STATIC_DRAW);
}

// TODO: Draw model
void draw_model()
{
	glUseProgram(programID);
	glBindVertexArray(VAID);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), BUFFER_OFFSET(0));

	//glm::mat4 Model = glm::mat4(1.0f);
	degree += 1.0f;
	position -= 0.005f;
	glm::mat4 Rotation = glm::rotate(degree, glm::vec3(0, 0, 1));
	glm::mat4 Translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, position, 0.0f));
	glm::mat4 Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.0f));
	glm::mat4 RBT = Translation * Rotation;
	glm::mat4 MVP = Projection * View * RBT * Scale;

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)g_vertex_buffer_data.size()); //just to avoid warning message...
	glDisableVertexAttribArray(0);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main(int argc, char* argv[])
{
	// Step 1: Initialization
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// TODO: GLFW create window and context
	window = glfwCreateWindow(1024, 768, "ORMEEHYUNGKEUN CHA:20156400 Lab 1", NULL, NULL);
	if (window == NULL)
	{
		return -1;
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// END

	// TODO: Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		return -1;
		glfwTerminate();
	}
	// END

	Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	View = glm::lookAt(glm::vec3(0, 0, 2),
				 				 glm::vec3(0, 0, 0),
								 glm::vec3(0, 1, 0));
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	// TODO: Initialize OpenGL and GLSL

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	programID = LoadShaders("VertexShader.glsl", "FragmentShader.glsl");
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// END
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	init_model();

	// Step 2: Main event loop
	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		draw_model();

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (!glfwWindowShouldClose(window));

	// Step 3: Termination
	g_vertex_buffer_data.clear();

	glDeleteBuffers(1, &VBID);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VAID);

	glfwTerminate();

	return 0;
}
