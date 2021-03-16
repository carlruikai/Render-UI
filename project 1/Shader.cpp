#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

#define GLEW_STATIC
#include<GL/glew.h>
#include<GLFW/glfw3.h>

//create vertex shader and fragement shader
Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	std::ifstream vertexFile;
	std::ifstream fragmentFile;
	std::stringstream vertexSStream;
	std::stringstream fragmentSStream;

	vertexFile.open(vertexPath); //打开文件
	fragmentFile.open(fragmentPath);
	vertexFile.exceptions(std::ifstream::failbit || std::ifstream::badbit);
	fragmentFile.exceptions(std::ifstream::failbit || std::ifstream::badbit);
	try
	{
		if (!vertexFile.is_open() || !fragmentFile.is_open())
		{
			throw std::exception("open file error");
		}
		vertexSStream << vertexFile.rdbuf(); //文件流
		fragmentSStream << fragmentFile.rdbuf();

		vertexString = vertexSStream.str(); //字符串流
		fragmentString = fragmentSStream.str();

		vertexSource = vertexString.c_str(); //转换成opengl可使用的源码
		fragmentSource = fragmentString.c_str();

		unsigned int vertex, fragment;
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertexSource, NULL);
		glCompileShader(vertex);

		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fragmentSource, NULL);
		glCompileShader(fragment);

		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	catch (const std::exception& ex)
	{
		printf(ex.what());
	}
}
