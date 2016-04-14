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
GLuint CBID;
GLuint BG_VAID;
GLuint BG_VBID;
GLuint BG_CBID;

std::vector<glm::vec3> g_vertex_buffer_data;
std::vector<glm::vec3> g_color_buffer_data;
std::vector<glm::vec3> bgm_vertex_buffer_data;
std::vector<glm::vec3> bgm_color_buffer_data;
std::vector<glm::mat4> snowflake;
std::vector<float> iYposition;
std::vector<float> iDegree;

glm::mat4 Projection;
glm::mat4 View;

double before = 0.0;
double after = 0.0;

double xpos, ypos; //cursor position
float xsize = 1024.0f; //window size
float ysize = 768.0f;
float aspect = xsize / ysize;

int i; //for iteration

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
	snowflake = std::vector<glm::mat4>();
	iYposition = std::vector<float>();
	iDegree = std::vector<float>();
	g_vertex_buffer_data = std::vector<glm::vec3>();
	g_color_buffer_data = std::vector<glm::vec3>();

	g_vertex_buffer_data.push_back(glm::vec3(-0.5f, -0.25f, 0.0f));
	g_vertex_buffer_data.push_back(glm::vec3(0.0f, sqrt(0.75) - 0.25f, 0.0f));
	g_vertex_buffer_data.push_back(glm::vec3(0.5f, -0.25f, 0.0f));

	koch_line(g_vertex_buffer_data[0], g_vertex_buffer_data[1], 5);
	koch_line(g_vertex_buffer_data[1], g_vertex_buffer_data[2], 5);
	koch_line(g_vertex_buffer_data[2], g_vertex_buffer_data[0], 5);

	for (i = 0; i < g_vertex_buffer_data.size(); i++)
		g_color_buffer_data.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

	printf("VBO_SIZE:%zd\n ", g_vertex_buffer_data.size());

	// Generates Vertex Array Objects in the GPU¡¯s memory and passes back their identifiers
	// Create a vertex array object that represents vertex attributes stored in a vertex buffer object.
	glGenVertexArrays(1, &VAID);
	glBindVertexArray(VAID);
	// Create and initialize a buffer object, Generates our buffers in the GPU¡¯s memory
	glGenBuffers(1, &VBID);
	glBindBuffer(GL_ARRAY_BUFFER, VBID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*g_vertex_buffer_data.size(), &g_vertex_buffer_data[0], GL_STATIC_DRAW);

	glGenBuffers(1, &CBID);
	glBindBuffer(GL_ARRAY_BUFFER, CBID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*g_color_buffer_data.size(), &g_color_buffer_data[0], GL_STATIC_DRAW);
}

void init_bg()
{
	bgm_vertex_buffer_data = std::vector<glm::vec3>();
	bgm_color_buffer_data = std::vector<glm::vec3>();

	bgm_vertex_buffer_data.push_back(glm::vec3(-0.2f, 0.1f, -0.1f));
	bgm_vertex_buffer_data.push_back(glm::vec3(0.6f, 0.6f, -0.1f));
	bgm_vertex_buffer_data.push_back(glm::vec3(1.0f, 0.2f, -0.1f));

	bgm_vertex_buffer_data.push_back(glm::vec3(1.0f, -1.0f, -0.1f));
	bgm_vertex_buffer_data.push_back(glm::vec3(-1.0f, -1.0f, -0.1f));

	bgm_vertex_buffer_data.push_back(glm::vec3(-1.0f, 0.0f, -0.1f));
	bgm_vertex_buffer_data.push_back(glm::vec3(-0.6f, 0.4f, -0.1f));

	for (i = 0; i < bgm_vertex_buffer_data.size(); i ++)
		bgm_color_buffer_data.push_back(glm::vec3(0.0f, 0.2f, 0.0f));
	
	glGenVertexArrays(1, &BG_VAID);
	glBindVertexArray(BG_VAID);
	glGenBuffers(1, &BG_VBID);
	glBindBuffer(GL_ARRAY_BUFFER, BG_VBID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*bgm_vertex_buffer_data.size(), &bgm_vertex_buffer_data[0], GL_STATIC_DRAW);

	//color buffering
	glGenBuffers(1, &BG_CBID);
	glBindBuffer(GL_ARRAY_BUFFER, BG_CBID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*bgm_color_buffer_data.size(), &bgm_color_buffer_data[0], GL_STATIC_DRAW);
}

// TODO: Draw model
void draw_model()
{
	for (i = 0; i < snowflake.size(); i++)
	{
		glUseProgram(programID);
		glBindVertexArray(VAID);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBID);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), BUFFER_OFFSET(0));

		//since fragment shader is modified, add color buffering
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, CBID);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), BUFFER_OFFSET(0));

		glm::mat4 Rotation = glm::rotate(iDegree[i], glm::vec3(0, 0, 1));
		glm::mat4 Translation = snowflake[i] * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, iYposition[i], 0.0f));
		glm::mat4 Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.0f));
		glm::mat4 RBT = Translation * Rotation;
		glm::mat4 MVP = Projection * View * RBT * Scale;
		iDegree[i] += 1.0f;
		iYposition[i] -= 0.005f;

		//When elements in vectors are removed too early, flickering occurs. 
		if (iYposition[i] < -2.5f)
		{
			//std::swap(snowflake[i], snowflake.back());
			//snowflake.pop_back();
			//std::swap(iYposition[i], iYposition.back());
			//iYposition.pop_back();
			//std::swap(iDegree[i], iDegree.back());
			//iDegree.pop_back();
			snowflake.erase(snowflake.begin());
			iYposition.erase(iYposition.begin());
			iDegree.erase(iDegree.begin());
		}

		GLuint MatrixID = glGetUniformLocation(programID, "MVP");
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)g_vertex_buffer_data.size()); //just to avoid warning message...
		glDisableVertexAttribArray(0);
	}
}

void draw_bg()
{
	glUseProgram(programID);
	glBindVertexArray(BG_VAID);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, BG_VBID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), BUFFER_OFFSET(0));

	//0 for vertices, 1 for coloring
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, BG_CBID);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), BUFFER_OFFSET(0));

	glm::mat4 Scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.0f));
	glm::mat4 MVP = Projection * View * Scale;

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)bgm_vertex_buffer_data.size());
	glDisableVertexAttribArray(0);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	xsize = (float)width;
	ysize = (float)height;
	aspect = xsize / ysize;
	printf("width:%f height:%f\n", xsize, ysize);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		glfwGetCursorPos(window, &xpos, &ypos);

		//clumsy conversion;
		if (xpos >= xsize / 2)
			xpos = (xpos / (xsize / 2)) - 1;
		else if (xpos < xsize / 2)
			xpos = (((((xsize / 2) - xpos) * 2) + xpos) / (xsize / 2) - 1) * -1;
		
		if (ypos >= ysize / 2)
			ypos = ((ypos / (ysize / 2)) - 1) * -1;
		else if (ypos < ysize / 2)
			ypos = (((((ysize / 2) - ypos) * 2) + ypos) / (ysize / 2) - 1);

		printf("X:%f Y:%f\n", xpos, ypos);

		snowflake.push_back(glm::translate(glm::mat4(1.0f), glm::vec3((float)xpos, (float)ypos / 2.5f, 0.0f)));
		iYposition.push_back((float)ypos / 2.5f);
		iDegree.push_back(0.0f);
		printf("inserted %zd\n", snowflake.size());
	}
}

int main(int argc, char* argv[])
{
	// Step 1: Initialization
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_SAMPLES, 16);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// TODO: GLFW create window and context
	window = glfwCreateWindow((int)xsize, (int)ysize, "ORMEEHYUNGKEUN CHA:20156400 Lab 1", NULL, NULL);
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

	//changed to orthographic projection, but still doesn't fit the scale
	Projection = glm::ortho(-1.0f, 1.0f, -1.0f / aspect, 1.0f / aspect, 0.1f, 100.0f);
	//Projection = glm::perspective(45.0f, aspect, -1.0f, 1.0f);
	View = glm::lookAt(glm::vec3(0, 0, 2),
				 				 glm::vec3(0, 0, 0),
								 glm::vec3(0, 1, 0));
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	// TODO: Initialize OpenGL and GLSL

	glClearColor(0.0f, 0.0f, 0.15f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	programID = LoadShaders("VertexShader.glsl", "FragmentShader.glsl");
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// END
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	init_model();
	init_bg();

	// Step 2: Main event loop
	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		before = glfwGetTime();
		draw_model();
		draw_bg();
		after = glfwGetTime();
		while ((before - after) > (1.0 / 60.0))
		{
			after = glfwGetTime();
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (!glfwWindowShouldClose(window));

	// Step 3: Termination
	g_vertex_buffer_data.clear();
	g_color_buffer_data.clear();
	bgm_vertex_buffer_data.clear();
	bgm_color_buffer_data.clear();

	glDeleteBuffers(1, &VBID);
	glDeleteBuffers(1, &CBID);
	glDeleteBuffers(1, &BG_VBID);
	glDeleteBuffers(1, &BG_CBID);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VAID);
	glDeleteVertexArrays(1, &BG_VAID);

	glfwTerminate();

	return 0;
}
