#define STB_IMPLEMENTATION
#include "stb_image.h"
#include "glad\glad.h"
#include "GLFW\glfw3.h"

#include <iostream>
#include <random>
#include <sstream>
#include <fstream>
#include <chrono>

#define GRID_SIZE 1024

struct Color
{
	unsigned char r, g, b, a;
	Color()
		: r{255}, g{255}, b{255}, a{255}
	{}

	Color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a)
		: r{_r}, g{_g}, b{_b}, a{_a}
	{}
};


static Color* buffer = (Color*)malloc(GRID_SIZE * GRID_SIZE * sizeof(Color));
/*
*  0--------1
*  |        |
*  |        |
*  2--------3
*/
static float verts[12] = 
{
	-1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f
};

static unsigned int idx[6] =
{
	0, 1, 2,
	1, 3, 2
};

static const char* vShader = "#version 460 core\n"
"layout(location = 0) in vec3 vert_pos;"
"out vec2 frag_texCoords;"
"void main(void){"
"gl_Position = vec4(vert_pos, 1.0);"
"frag_texCoords = vec2((vert_pos.x+1.0)/8, (vert_pos.y+1.0)/8);}";

static const char* fShader =
"#version 460 core\n"
"in vec2 frag_texCoords;"
"out vec4 outColor;"
"uniform sampler2D grid;"
"void main(void){"
"outColor = texture(grid, frag_texCoords);}";


void resetBuffer(Color* buffer, GLuint texture)
{
	std::srand(std::time(0));
	for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++)
	{
		if (std::rand() % 2)
		{
			buffer[i].r = 255;
			buffer[i].g = 255;
			buffer[i].b = 255;
			buffer[i].a = 255;
		}
		else
		{
			buffer[i].r = 0;
			buffer[i].g = 0;
			buffer[i].b = 0;
			buffer[i].a = 255;
		}
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, GRID_SIZE, GRID_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
}

int main()
{
	
	for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++)
	{
		if (std::rand() % 2)
		{
			buffer[i].r = 255;
			buffer[i].g = 255;
			buffer[i].b = 255;
			buffer[i].a = 255;
		}
		else
		{
			buffer[i].r = 0;
			buffer[i].g = 0;
			buffer[i].b = 0;
			buffer[i].a = 255;
		}
	}
	

	//buffer[129] = Color(0, 0, 0, 255);
	//buffer[130] = Color(0, 0, 0, 255);
	//buffer[257] = Color(0, 0, 0, 255);
	//buffer[258] = Color(0, 0, 0, 255);



	int success = glfwInit();
	if (!success)
	{
		printf("glfw init failed\n");
		return -1;
	}
	
	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Compute Shader Test", nullptr, nullptr);
	if (!window)
	{
		printf("window init failed\n");
		return -1;
	}
	glfwMakeContextCurrent(window);

	success = gladLoadGL();
	if (!success)
	{
		printf("Glad failed to init\n");
		return -1;
	}


	GLuint textures[2] = {0,0};
	glGenTextures(2, textures);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, GRID_SIZE, GRID_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	glBindImageTexture(0, textures[0], 0, false, 0, GL_READ_WRITE, GL_RGBA8);

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, GRID_SIZE, GRID_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	glBindImageTexture(1, textures[1], 0, false, 0, GL_READ_WRITE, GL_RGBA8);
	


	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glActiveTexture(GL_TEXTURE0);


	GLuint vao, vbo, vbi;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vbi);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbi);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, false, 0);


	GLuint progID, vertID, fragID;
	progID = glCreateProgram();
	vertID = glCreateShader(GL_VERTEX_SHADER);
	fragID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertID, 1, &vShader, NULL);
	glShaderSource(fragID, 1, &fShader, NULL);
	glCompileShader(vertID);
	glCompileShader(fragID);
	GLint status;
	char infoLog[1024];
	if(glGetShaderiv(vertID, GL_COMPILE_STATUS, &status); !status)
	{
		glGetShaderInfoLog(vertID, 1024, NULL, infoLog);
		printf(infoLog);
		printf("Could not compile vert Shader\n");
	}

	if (glGetShaderiv(fragID, GL_COMPILE_STATUS, &status); !status)
	{
		glGetShaderInfoLog(fragID, 1024, NULL, infoLog);
		printf(infoLog);
		printf("Could not compile frag shader\n");
	}

	glAttachShader(progID, vertID);
	glAttachShader(progID, fragID);
	glLinkProgram(progID);
	glUseProgram(progID);

	GLuint cprogID, compID;
	cprogID = glCreateProgram();
	compID = glCreateShader(GL_COMPUTE_SHADER);

	{
		std::ifstream vStream{ "src/computeShader.comp", std::ios::in };
		std::stringstream ss;
		std::string line;
		while (!vStream.eof())
		{
			std::getline(vStream, line);
			ss << line << '\n';
		}
		const std::string src = ss.str();
		const char* source = src.c_str();
		glShaderSource(compID, 1, &source, NULL);
		glCompileShader(compID);
		if (glGetShaderiv(compID, GL_COMPILE_STATUS, &status); !status)
		{
			glGetShaderInfoLog(compID, 1024, NULL, infoLog);
			printf(infoLog);
			printf("Could not compile compute shader\n");
		}
	}


	glAttachShader(cprogID, compID);
	glLinkProgram(cprogID);



	int max_iterations = 1, iterations = 0;
	float gen_rate = 1.0f/60.0f;
	auto time = std::chrono::steady_clock::now();
	

	//BE ABLE TO MOVE THE GRID


	//glViewport(0, 0, GRID_SIZE, GRID_SIZE);
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		auto dt = std::chrono::steady_clock::now() - time;
		auto dtmSecs = std::chrono::duration_cast<std::chrono::milliseconds>(dt);
		float delta = (float)dtmSecs.count()/1000.0f;






		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, GLFW_TRUE);
		if (glfwGetKey(window, GLFW_KEY_Q))
		{
			resetBuffer(buffer, textures[0]);
		}
			//if (glfwGetKey(window, GLFW_KEY_E) && delta >= gen_rate) max_iterations++;
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		if (iterations<max_iterations && delta>=gen_rate)
		{
			glUseProgram(cprogID);
			glBindImageTexture(0, textures[0], 0, false, 0, GL_READ_ONLY, GL_RGBA8);
			glBindImageTexture(1, textures[1], 0, false, 0, GL_WRITE_ONLY, GL_RGBA8);
			
			glDispatchCompute(GRID_SIZE, GRID_SIZE, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


			GLuint temp = textures[0];
			textures[0] = textures[1];
			textures[1] = temp;


			glUseProgram(progID);
			glBindTexture(GL_TEXTURE_2D, textures[0]);
			glActiveTexture(GL_TEXTURE0);
			max_iterations++;
			time = std::chrono::steady_clock::now();
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
		while (glGetError()) printf("Error occurred\n");
	}
	

	glfwTerminate();
	std::cin.get();
	return 0;
}