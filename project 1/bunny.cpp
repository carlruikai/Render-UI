#define GLEW_STATIC
#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<soil.h>

#include "Shader.h"
#include "sphere_camera.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imstb_rectpack.h"
#include "imstb_textedit.h"
#include "imstb_truetype.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

const GLuint WIDTH = 1024, HEIGHT = 768;
const float PI = 3.14;

Camera camera(glm::vec3(0.0f, 0.0f, 4.0f));
glm::vec3 lightPos(-1.0f, -1.0f, -1.0f);
glm::mat4 model(1.0f);

unsigned int vertice_size = 0; // 顶点数量
unsigned int face_size = 0; //三角形数量
float radius = 0.0f; //camera的旋转半径
glm::vec3 object_rotate_axis(1.0f, 0.0f, 0.0f);

bool keys[10000];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

//球体
std::vector<float> vertices;
std::vector<int> indices;
std::vector<float> normals;

//圆锥
std::vector<float> vertices1;
std::vector<int> indices1;
std::vector <float> normals1;

//圆柱
std::vector<float> vertices2;
std::vector<int> indices2;
std::vector<float> normals2;

//xy平面圆
std::vector<float> circle_vertices;
std::vector<int> circle_indices;

//xz平面圆
std::vector<float> circle_vertices1;
std::vector<int> circle_indices1;

//yz平面圆
std::vector<float> circle_vertices2;
std::vector<int> circle_indices2;

std::vector<GLfloat> bunny_vertices;
std::vector<GLfloat> bunny_normals;
std::vector<GLfloat> selected_points;
std::vector<int> selected_indices;

int draw_sphere(float radius, int sectorCount, int stackCount);
int draw_cylinder(float radius, float height, int sectorcount);
int draw_real_cylinder(float radius, float height, int sectorcount);
int DrawCircle(GLfloat x, GLfloat y, GLfloat radius, int num_segments);
int drawCircle(GLfloat x, GLfloat z, GLfloat radius, int num_segments);
int Drawcircle(GLfloat y, GLfloat z, GLfloat radius, int num_segments);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void camera_movement();
void mouse_movement(GLFWwindow* window);
int select_points(double initial_pickbox_x, double initial_pickbox_y, double end_pickbox_x, double end_pickbox_y);


float xy_min_distance = 9999.0;
float xz_min_distance = 9999.0;
float yz_min_distance = 9999.0;
float x_mouse_move = 0.0;
float y_mouse_move = 0.0;
float z_mouse_move = 0.0;

glm::mat4 circle_model(1.0f);
glm::mat4 circle_model1(1.0f);
glm::mat4 circle_model2(1.0f);

double initial_pickbox_x = 0.0, initial_pickbox_y = 0.0, end_pickbox_x = 0.0, end_pickbox_y = 0.0;
int selected_points_VAO;
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "bunny", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	
	glfwSetKeyCallback(window, key_callback);
	//glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glewExperimental = GL_TRUE;
	glewInit();
	
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glEnable(GL_DEPTH_TEST);

	

	Shader bunnyshader = Shader("bunnyvertex.txt", "bunnyfragment.txt");
	Shader sphereshader = Shader("spherevertex.txt", "spherefragment.txt");
	Shader cylindershader_1 = Shader("cylindervertex.txt", "cylinderfragment.txt");
	Shader cylindershader_2 = Shader("cylindervertex.txt", "cylinderfragment.txt");
	Shader cylindershader_3 = Shader("cylindervertex.txt", "cylinderfragment.txt");

	Shader cylindershader_4 = Shader("spherevertex.txt", "spherefragment.txt");
	Shader cylindershader_5 = Shader("spherevertex.txt", "spherefragment.txt");
	Shader cylindershader_6 = Shader("spherevertex.txt", "spherefragment.txt");
	Shader real_cylindershader_1 = Shader("spherevertex.txt", "spherefragment.txt");
	Shader real_cylindershader_2 = Shader("spherevertex.txt", "spherefragment.txt");
	Shader real_cylindershader_3 = Shader("spherevertex.txt", "spherefragment.txt");

	Shader circleshader = Shader("cylindervertex.txt", "cylinderfragment.txt");

	GLfloat Vertex[3];
	GLfloat Face[3];
	GLfloat Normal[3];
	std::vector<GLfloat> Vertices;
	std::vector<GLuint> Faces;
	std::vector<glm::vec3> Normals;
	std::vector<float> Tri_areas;
	std::vector <GLfloat> v_Normals;
	std::string Line;
	std::ifstream File;
	std::map<int, std::vector<int>> refer;
	int line = 0;

	File.open("bunny.obj");
	float x_min = 0.0f, x_max = 0.0f, y_min = 0.0f, y_max = 0.0f, z_min = 0.0f, z_max = 0.0f;
	while (std::getline(File, Line)) {
		if (Line == "" || Line[0] == '#')
			continue;

		if (Line.c_str()[0] == 'v') {
			vertice_size++;
			sscanf_s(Line.c_str(), "%*s %f %f %f", &Vertex[0], &Vertex[1], &Vertex[2]);
			if (Vertex[0] > x_max)
				x_max = Vertex[0];
			else if (Vertex[0] < x_min)
				x_min = Vertex[0];
			if (Vertex[1] > y_max)
				y_max = Vertex[1];
			else if (Vertex[1] < y_min)
				y_min = Vertex[1];
			if (Vertex[2] > z_max)
				z_max = Vertex[2];
			else if (Vertex[2] < z_min)
				z_min = Vertex[2];
			Vertices.push_back(Vertex[0]); 
			Vertices.push_back(Vertex[1]); 
			Vertices.push_back(Vertex[2]); 
			// vertex.x, vertex.y, vertex.z
			bunny_vertices.push_back(Vertex[0]);
			bunny_vertices.push_back(Vertex[1]);
			bunny_vertices.push_back(Vertex[2]);
		}

		else if (Line.c_str()[0] == 'f') {
			face_size++;
			sscanf_s(Line.c_str(), "%*s %f %f %f", &Face[0], &Face[1], &Face[2]);
			Faces.push_back(Face[0] - 1); //在文件中顶点顺序从1开始
			Faces.push_back(Face[1] - 1);
			Faces.push_back(Face[2] - 1);
			//face.vertex1, face.vertex2, face.vertex3
			refer[(Face[0] - 1)].push_back(line); //每个顶点所相邻的三角形序号
			refer[(Face[1] - 1)].push_back(line);
			refer[(Face[2] - 1)].push_back(line);
			line++; 
		};

	};
	radius = x_max - x_min;
	float temp = y_max - y_min;
	if (temp > radius)
		radius = temp;
	temp = z_max - z_min;
	if (temp > radius)
		radius = temp;
	radius = radius / 2.0 + 2.0;

	float x_medium = (x_max + x_min) / 2.0; // 物体x轴中心
	float y_medium = (y_max + y_min) / 2.0; //物体y轴中心
	float z_medium = (z_max + z_min) / 2.0; //物体z轴中心


	//计算每个面的法向量和面积
	for (int i = 0; i < face_size; i++) {
		glm::vec3 p1(Vertices[3 * Faces[3 * i]] , Vertices[3 * Faces[3 * i] + 1], Vertices[3 * Faces[3 * i] + 2]);
		glm::vec3 p2(Vertices[3 * Faces[3 * i + 1]], Vertices[3 * Faces[3 * i + 1] + 1], Vertices[3 * Faces[3 * i + 1] + 2]);
		glm::vec3 p3(Vertices[3 * Faces[3 * i + 2]], Vertices[3 * Faces[3 * i + 2] + 1], Vertices[3 * Faces[3 * i + 2] + 2]);
		glm::vec3 normal = glm::normalize(glm::cross(glm::normalize(p2 - p1), glm::normalize(p3 - p1)));
		Normals.push_back(normal); //Normals.size() == Faces.size()
		float area = glm::length(glm::cross(p2 - p1, p3 - p1)) / 2.0;
		Tri_areas.push_back(area); // area of triangles
	}
	int a = 0;
	glm::vec3 vertex_normal;
	glm::vec3 sum_weighted_normal(0.0f);
	for (int i = 0; i < vertice_size; i++) {
		sum_weighted_normal = glm::vec3(0.0f, 0.0f, 0.0f);
		for (auto iter = refer[i].cbegin(); iter != refer[i].cend(); iter++) {
			sum_weighted_normal = sum_weighted_normal + Tri_areas[*iter] * Normals[*iter];
		}
		vertex_normal = glm::normalize(sum_weighted_normal);
		v_Normals.push_back(vertex_normal.x); //normal vectors of vertices
		v_Normals.push_back(vertex_normal.y);
		v_Normals.push_back(vertex_normal.z);
		bunny_normals.push_back(vertex_normal.x);
		bunny_normals.push_back(vertex_normal.y);
		bunny_normals.push_back(vertex_normal.z);
	}

	GLfloat texcoords[209010];
	for (int i = 0; i < 209010; i++)
	{
		if (i % 6 == 0)
			texcoords[i] = 0.0f;
		if (i % 6 == 1)
			texcoords[i] = 0.0f;
		if (i % 6 == 2)
			texcoords[i] = 0.5f;
		if (i % 6 == 3)
			texcoords[i] = 1.0f;
		if (i % 6 == 4)
			texcoords[i] = 1.0f;
		if (i % 6 == 5)
			texcoords[i] = 0.0f;
	}

	GLuint vertex_EBO, location_VBO, vertex_VAO, color_VBO, texture_VBO;
	glGenVertexArrays(1, &vertex_VAO);
	glGenBuffers(1, &location_VBO);
	glGenBuffers(1, &vertex_EBO);
	glGenBuffers(1, &color_VBO);
	glGenBuffers(1, &texture_VBO);

	glBindVertexArray(vertex_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, location_VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(GLfloat), &Vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, color_VBO);
	glBufferData(GL_ARRAY_BUFFER, v_Normals.size() * sizeof(GLfloat), &v_Normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, texture_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Faces.size() * sizeof(GLuint), &Faces[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	int sphere_VAO = draw_sphere(1.0f, 36, 18);
	int cylinder_VAO = draw_cylinder(1.0f, 2.0f, 36); //圆锥
	int real_cylinder_VAO = draw_real_cylinder(0.5f, 3.0f, 36);
	int circle_VAO = DrawCircle(0.0f, 0.0f, 1.5f, 100);
	int circle_VAO1 = drawCircle(0.0f, 0.0f, 1.5f, 100);
	int circle_VAO2 = Drawcircle(0.0f, 0.0f, 1.5f, 100);
	

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int texwidth, texheight;
	unsigned char* image = SOIL_load_image("matrix.jpg", &texwidth, &texheight, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texwidth, texheight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	


	while (!glfwWindowShouldClose(window))
	{	
		glViewport(0, 0, width, height);
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;








		glfwPollEvents();
		camera_movement();
		mouse_movement(window);


		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glBindTexture(GL_TEXTURE_2D, texture);
		glUseProgram(bunnyshader.ID);


		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);



		glUniformMatrix4fv(glGetUniformLocation(bunnyshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(bunnyshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(bunnyshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glUniform3f(glGetUniformLocation(bunnyshader.ID, "objectColor"), 1.0f, 0.5f, 0.31f);
		glUniform3f(glGetUniformLocation(bunnyshader.ID, "lightColor"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(bunnyshader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(bunnyshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		glUniform3f(glGetUniformLocation(bunnyshader.ID, "direColor"), 0.3f, 0.4f, 0.5f);
		glUniform3f(glGetUniformLocation(bunnyshader.ID, "direDir"), 3.0f, -4.0f, -5.0f);

		glBindVertexArray(vertex_VAO);
		glDrawElements(GL_TRIANGLES, Faces.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glUseProgram(sphereshader.ID);
		glm::mat4 sphere_model(1.0f);
		sphere_model = glm::translate(sphere_model, lightPos);
		sphere_model = glm::scale(sphere_model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(sphere_model));
		glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), 0.3f, 0.4f, 0.5f);
		glUniform3f(glGetUniformLocation(sphereshader.ID, "objectColor"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(sphereshader.ID, "lightPos"), -2.0f, -1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(sphereshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		glBindVertexArray(sphere_VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glUseProgram(cylindershader_1.ID);
		glm::mat4 cylinder_model_1(1.0f);
		cylinder_model_1 = glm::translate(cylinder_model_1, glm::vec3(2.0f, 2.0f, -2.0f));
		cylinder_model_1 = glm::rotate(cylinder_model_1, glm::radians(135.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		cylinder_model_1 = glm::scale(cylinder_model_1, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(glGetUniformLocation(cylindershader_1.ID, "model"), 1, GL_FALSE, glm::value_ptr(cylinder_model_1));
		glUniformMatrix4fv(glGetUniformLocation(cylindershader_1.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(cylindershader_1.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(glGetUniformLocation(cylindershader_1.ID, "lightColor"), 0.6f, 0.8f, 1.0f);
		glBindVertexArray(cylinder_VAO);
		glDrawElements(GL_TRIANGLES, indices1.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glUseProgram(cylindershader_2.ID);
		glm::mat4 cylinder_model_2(1.0f);
		cylinder_model_2 = glm::translate(cylinder_model_2, glm::vec3(0.0f, 2.0f, -2.0f));
		cylinder_model_2 = glm::rotate(cylinder_model_2, glm::radians(135.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		cylinder_model_2 = glm::scale(cylinder_model_2, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(glGetUniformLocation(cylindershader_2.ID, "model"), 1, GL_FALSE, glm::value_ptr(cylinder_model_2));
		glUniformMatrix4fv(glGetUniformLocation(cylindershader_2.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(cylindershader_2.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(glGetUniformLocation(cylindershader_2.ID, "lightColor"), 0.6f, 0.8f, 1.0f);
		glBindVertexArray(cylinder_VAO);
		glDrawElements(GL_TRIANGLES, indices1.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glUseProgram(cylindershader_3.ID);
		glm::mat4 cylinder_model_3(1.0f);
		cylinder_model_3 = glm::translate(cylinder_model_3, glm::vec3(-2.0f, 2.0f, -2.0f));
		cylinder_model_3 = glm::rotate(cylinder_model_3, glm::radians(135.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		cylinder_model_3 = glm::scale(cylinder_model_3, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(glGetUniformLocation(cylindershader_3.ID, "model"), 1, GL_FALSE, glm::value_ptr(cylinder_model_3));
		glUniformMatrix4fv(glGetUniformLocation(cylindershader_3.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(cylindershader_3.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(glGetUniformLocation(cylindershader_3.ID, "lightColor"), 0.6f, 0.8f, 1.0f);
		glBindVertexArray(cylinder_VAO);
		glDrawElements(GL_TRIANGLES, indices1.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glUseProgram(circleshader.ID);
		glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(circle_model));
		glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		if(xy_min_distance < 0.0005 && xy_min_distance < xz_min_distance && xy_min_distance < yz_min_distance)
			glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 0.5f, 0.5f, 0.5f);
		else 
			glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 0.0f, 0.0f, 1.0f);
		glBindVertexArray(circle_VAO);
		glDrawElements(GL_LINE_LOOP, circle_indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		
		glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(circle_model1));
		glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		if (xz_min_distance < 0.0005 && xz_min_distance < xy_min_distance && xz_min_distance < yz_min_distance)
			glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 0.5f, 0.5f, 0.5f);
		else
			glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 1.0f, 0.0f, 0.0f);
		glBindVertexArray(circle_VAO1);
		glDrawElements(GL_LINE_LOOP, circle_indices1.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);


		glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(circle_model2));
		glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		if (yz_min_distance < 0.0005 && yz_min_distance < xz_min_distance && yz_min_distance < xy_min_distance)
			glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 0.5f, 0.5f, 0.5f);
		else
			glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 0.0f, 1.0f, 0.0f);
		glBindVertexArray(circle_VAO2);
		glDrawElements(GL_LINE_LOOP, circle_indices2.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);


		glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 1.0f, 1.0f, 0.0f);
		glBindVertexArray(selected_points_VAO);
		glDrawElements(GL_POINTS, selected_indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glViewport(0, 0, width / 4, height / 4);

		glUseProgram(cylindershader_4.ID);
		glm::mat4 cylinder_model_4(1.0f);
		cylinder_model_4 = glm::rotate(cylinder_model_4, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		cylinder_model_4 = glm::translate(cylinder_model_4, glm::vec3(1.0f, 0.5f, 0.0f));
		cylinder_model_4 = glm::scale(cylinder_model_4, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(glGetUniformLocation(cylindershader_4.ID, "model"), 1, GL_FALSE, glm::value_ptr(cylinder_model_4));
		glUniformMatrix4fv(glGetUniformLocation(cylindershader_4.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(cylindershader_4.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(glGetUniformLocation(cylindershader_4.ID, "lightColor"), 0.1f, 0.2f, 0.3f);
		glUniform3f(glGetUniformLocation(cylindershader_4.ID, "objectColor"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(cylindershader_4.ID, "lightPos"), -2.0f, -1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(cylindershader_4.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		glBindVertexArray(cylinder_VAO);
		glDrawElements(GL_TRIANGLES, indices1.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glUseProgram(cylindershader_5.ID);
		glm::mat4 cylinder_model_5(1.0f);
		cylinder_model_5 = glm::translate(cylinder_model_5, glm::vec3(-1.0f, 0.5f, 0.0f));
		cylinder_model_5 = glm::scale(cylinder_model_5, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(glGetUniformLocation(cylindershader_5.ID, "model"), 1, GL_FALSE, glm::value_ptr(cylinder_model_5));
		glUniformMatrix4fv(glGetUniformLocation(cylindershader_5.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(cylindershader_5.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(glGetUniformLocation(cylindershader_5.ID, "lightColor"), 0.4f, 0.5f, 0.6f);
		glUniform3f(glGetUniformLocation(cylindershader_5.ID, "objectColor"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(cylindershader_5.ID, "lightPos"), 2.0f, -1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(cylindershader_5.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);

		glBindVertexArray(cylinder_VAO);
		glDrawElements(GL_TRIANGLES, indices1.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glUseProgram(cylindershader_6.ID);
		glm::mat4 cylinder_model_6(1.0f);
		cylinder_model_6 = glm::rotate(cylinder_model_6, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		cylinder_model_6 = glm::translate(cylinder_model_6, glm::vec3(-1.0f, 1.5f, 1.0f));
		cylinder_model_6 = glm::scale(cylinder_model_6, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(glGetUniformLocation(cylindershader_6.ID, "model"), 1, GL_FALSE, glm::value_ptr(cylinder_model_6));
		glUniformMatrix4fv(glGetUniformLocation(cylindershader_6.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(cylindershader_6.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(glGetUniformLocation(cylindershader_6.ID, "lightColor"), 0.7f, 0.8f, 0.9f);
		glUniform3f(glGetUniformLocation(cylindershader_6.ID, "objectColor"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(cylindershader_6.ID, "lightPos"), -2.0f, 1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(cylindershader_6.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);

		glBindVertexArray(cylinder_VAO);
		glDrawElements(GL_TRIANGLES, indices1.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glUseProgram(real_cylindershader_1.ID);
		glm::mat4 real_cylinder_model_1(1.0f);
		real_cylinder_model_1 = glm::rotate(real_cylinder_model_1, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		real_cylinder_model_1 = glm::translate(real_cylinder_model_1, glm::vec3(1.0f, -0.5f, 0.0f));
		real_cylinder_model_1 = glm::scale(real_cylinder_model_1, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(glGetUniformLocation(real_cylindershader_1.ID, "model"), 1, GL_FALSE, glm::value_ptr(real_cylinder_model_1));
		glUniformMatrix4fv(glGetUniformLocation(real_cylindershader_1.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(real_cylindershader_1.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(glGetUniformLocation(real_cylindershader_1.ID, "lightColor"), 0.1f, 0.2f, 0.3f);
		glUniform3f(glGetUniformLocation(real_cylindershader_1.ID, "objectColor"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(real_cylindershader_1.ID, "lightPos"), -2.0f, -1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(real_cylindershader_1.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		glBindVertexArray(real_cylinder_VAO);
		glDrawElements(GL_TRIANGLES, indices2.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glUseProgram(real_cylindershader_2.ID);
		glm::mat4 real_cylinder_model_2(1.0f);
		real_cylinder_model_2 = glm::translate(real_cylinder_model_2, glm::vec3(-1.0f, -0.5f, 0.0f));
		real_cylinder_model_2 = glm::scale(real_cylinder_model_2, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(glGetUniformLocation(real_cylindershader_2.ID, "model"), 1, GL_FALSE, glm::value_ptr(real_cylinder_model_2));
		glUniformMatrix4fv(glGetUniformLocation(real_cylindershader_2.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(real_cylindershader_2.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(glGetUniformLocation(real_cylindershader_2.ID, "lightColor"), 0.4f, 0.5f, 0.6f);
		glUniform3f(glGetUniformLocation(real_cylindershader_2.ID, "objectColor"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(real_cylindershader_2.ID, "lightPos"), 2.0f, -1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(real_cylindershader_2.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		glBindVertexArray(real_cylinder_VAO);
		glDrawElements(GL_TRIANGLES, indices2.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glUseProgram(real_cylindershader_3.ID);
		glm::mat4 real_cylinder_model_3(1.0f);
		real_cylinder_model_3 = glm::rotate(real_cylinder_model_3, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		real_cylinder_model_3 = glm::translate(real_cylinder_model_3, glm::vec3(-1.0f, 0.5f, 1.0f));
		real_cylinder_model_3 = glm::scale(real_cylinder_model_3, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(glGetUniformLocation(real_cylindershader_3.ID, "model"), 1, GL_FALSE, glm::value_ptr(real_cylinder_model_3));
		glUniformMatrix4fv(glGetUniformLocation(real_cylindershader_3.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(real_cylindershader_3.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(glGetUniformLocation(real_cylindershader_3.ID, "lightColor"), 0.7f, 0.8f, 0.9f);
		glUniform3f(glGetUniformLocation(real_cylindershader_3.ID, "objectColor"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(real_cylindershader_3.ID, "lightPos"), -2.0f, 1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(real_cylindershader_3.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		glBindVertexArray(real_cylinder_VAO);
		glDrawElements(GL_TRIANGLES, indices2.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);


		glUseProgram(sphereshader.ID);
		sphere_model = glm::mat4(1.0f);
		sphere_model = glm::translate(sphere_model, glm::vec3(-1.0f, -1.0f, 0.0f));
		sphere_model = glm::scale(sphere_model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(sphere_model));
		glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), 0.3f, 0.4f, 0.5f);
		glUniform3f(glGetUniformLocation(sphereshader.ID, "objectColor"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(sphereshader.ID, "lightPos"), -2.0f, -1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(sphereshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		glBindVertexArray(sphere_VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);


		
		glfwSwapBuffers(window);
	}
	


	glDeleteVertexArrays(1, &vertex_VAO);
	glDeleteBuffers(1, &location_VBO);
	glDeleteBuffers(1, &color_VBO);
	glDeleteBuffers(1, &vertex_EBO);
	glDeleteBuffers(1, &texture_VBO);
	glfwTerminate();
	return 0;
}


//画球体
int draw_sphere(float radius, int sectorCount, int stackCount)
{

	float x, y, z, xz;
	float nx, ny, nz;
	float sectorStep = 2 * acos(-1) / sectorCount;
	float stackStep = acos(-1) / stackCount;
	float lengthInv = 1.0f / radius;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stackCount; i++)
	{
		stackAngle = acos(-1) / 2 - i * stackStep;
		xz = radius * cosf(stackAngle);
		y = radius * sinf(stackAngle);

		for (int j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;
			x = xz * sinf(sectorAngle);
			z = xz * cosf(sectorAngle);
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);

			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			normals.push_back(nx);
			normals.push_back(ny);
			normals.push_back(nz);
		}
	}

	int k1, k2;
	for (int i = 0; i < stackCount; i++)
	{
		k1 = i * (sectorCount + 1);
		k2 = k1 + sectorCount + 1;
		for (int j = 0; j < sectorCount; j++, k1++, k2++)
		{
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			if (i != (stackCount - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	GLuint sphere_VAO, sphere_VBO, sphere_EBO, sphere_nVBO;
	glGenVertexArrays(1, &sphere_VAO);
	glGenBuffers(1, &sphere_VBO);
	glGenBuffers(1, &sphere_EBO);
	glGenBuffers(1, &sphere_nVBO);

	glBindVertexArray(sphere_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, sphere_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, sphere_nVBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	return sphere_VAO;
}

//画圆柱底部的圆
std::vector<float> getunitcirclevertices(int sectorcount)
{
	float sectorstep = 2 * acos(-1) / sectorcount;
	float sectorangle;
	std::vector<float> unitcirclevertices;
	for (int i = 0; i <= sectorcount; ++i)
	{
		sectorangle = i * sectorstep;
		unitcirclevertices.push_back(cos(sectorangle));
		unitcirclevertices.push_back(0);
		unitcirclevertices.push_back(-sin(sectorangle));
	}
	return unitcirclevertices;
}

//画圆锥
int draw_cylinder(float radius, float height, int sectorcount)
{	
	float real_radius = radius;
	std::vector<float> unitvertices = getunitcirclevertices(sectorcount);
	for (int i = 0; i < 2; i++)
	{
		float h = -height / 2.0 + i * height;
		if (h == -height / 2.0)
			real_radius = radius;
		if (h == height / 2.0)
			real_radius = 0;
		for (int j = 0, k = 0; j <= sectorcount; j++, k += 3)
		{
			float ux = unitvertices[k];
			float uy = unitvertices[k + 1];
			float uz = unitvertices[k + 2];
			vertices1.push_back(ux * real_radius);
			vertices1.push_back(h);
			vertices1.push_back(uz * real_radius);

			normals1.push_back(ux);
			normals1.push_back(uz);
			normals1.push_back(uy);
		}
	}
	int basecenterindex = (int)vertices1.size() / 3;
	int topcenterindex = basecenterindex + sectorcount + 1;
	for (int i = 0; i < 2; i++)
	{
		float h = -height / 2.0 + i * height;
		float ny = -1 + i * 2;
		if (h == -height / 2.0 )
			real_radius = radius;
		if (h == height / 2.0 )
			real_radius = 0;
		vertices1.push_back(0);
		vertices1.push_back(h);
		vertices1.push_back(0);

		normals1.push_back(0);
		normals1.push_back(ny);
		normals1.push_back(0);

		for (int j = 0, k = 0; j < sectorcount; j++, k+=3)
		{
			float ux = unitvertices[k];
			float uz = unitvertices[k + 2];
			vertices1.push_back(ux * real_radius);
			vertices1.push_back(h);
			vertices1.push_back(uz * real_radius);

			normals1.push_back(0);
			normals1.push_back(ny);
			normals1.push_back(0);
		}
	}

	int k1 = 0;
	int k2 = sectorcount + 1;
	for (int i = 0; i < sectorcount; i++, k1++, k2++)
	{
		indices1.push_back(k1);
		indices1.push_back(k1 + 1);
		indices1.push_back(k2);
		indices1.push_back(k2);
		indices1.push_back(k1 + 1);
		indices1.push_back(k2 + 1);
	}

	for (int i = 0, k = basecenterindex + 1; i < sectorcount; i++, k++)
	{
		if (i < sectorcount - 1)
		{
			indices1.push_back(basecenterindex);
			indices1.push_back(k + 1);
			indices1.push_back(k);
		}
		else
		{
			indices1.push_back(basecenterindex);
			indices1.push_back(basecenterindex + 1);
			indices1.push_back(k);
		}
	}

	for (int i = 0, k = topcenterindex + 1; i < sectorcount; i++, k++)
	{
		if (i < sectorcount - 1)
		{
			indices1.push_back(topcenterindex);
			indices1.push_back(k);
			indices1.push_back(k + 1);
		}
		else
		{
			indices1.push_back(topcenterindex);
			indices1.push_back(k);
			indices1.push_back(topcenterindex + 1);
		}
	}

	GLuint cylinder_VAO, cylinder_VBO, cylinder_color_VBO, cylinder_EBO;
	glGenVertexArrays(1, &cylinder_VAO);
	glGenBuffers(1, &cylinder_VBO);
	glGenBuffers(1, &cylinder_EBO);
	glGenBuffers(1, &cylinder_color_VBO);

	glBindVertexArray(cylinder_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, cylinder_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices1.size() * sizeof(float), &vertices1[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, cylinder_color_VBO);
	glBufferData(GL_ARRAY_BUFFER, normals1.size() * sizeof(float), &normals1[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1);
	

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinder_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices1.size() * sizeof(int), &indices1[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	return cylinder_VAO;
}


//画圆柱
int draw_real_cylinder(float radius, float height, int sectorcount)
{
	float real_radius = radius;
	std::vector<float> unitvertices = getunitcirclevertices(sectorcount);
	for (int i = 0; i < 2; i++)
	{
		float h = -height / 2.0 + i * height;
		for (int j = 0, k = 0; j <= sectorcount; j++, k += 3)
		{
			float ux = unitvertices[k];
			float uy = unitvertices[k + 1];
			float uz = unitvertices[k + 2];
			vertices2.push_back(ux * real_radius);
			vertices2.push_back(h);
			vertices2.push_back(uz * real_radius);

			normals2.push_back(ux);
			normals2.push_back(uz);
			normals2.push_back(uy);
		}
	}
	int basecenterindex = (int)vertices2.size() / 3;
	int topcenterindex = basecenterindex + sectorcount + 1;
	for (int i = 0; i < 2; i++)
	{
		float h = -height / 2.0 + i * height;
		float ny = -1 + i * 2;
		vertices2.push_back(0);
		vertices2.push_back(h);
		vertices2.push_back(0);

		normals2.push_back(0);
		normals2.push_back(ny);
		normals2.push_back(0);

		for (int j = 0, k = 0; j < sectorcount; j++, k += 3)
		{
			float ux = unitvertices[k];
			float uz = unitvertices[k + 2];
			vertices2.push_back(ux * real_radius);
			vertices2.push_back(h);
			vertices2.push_back(uz * real_radius);

			normals2.push_back(0);
			normals2.push_back(ny);
			normals2.push_back(0);
		}
	}

	int k1 = 0;
	int k2 = sectorcount + 1;
	for (int i = 0; i < sectorcount; i++, k1++, k2++)
	{
		indices2.push_back(k1);
		indices2.push_back(k1 + 1);
		indices2.push_back(k2);
		indices2.push_back(k2);
		indices2.push_back(k1 + 1);
		indices2.push_back(k2 + 1);
	}

	for (int i = 0, k = basecenterindex + 1; i < sectorcount; i++, k++)
	{
		if (i < sectorcount - 1)
		{
			indices2.push_back(basecenterindex);
			indices2.push_back(k + 1);
			indices2.push_back(k);
		}
		else
		{
			indices2.push_back(basecenterindex);
			indices2.push_back(basecenterindex + 1);
			indices2.push_back(k);
		}
	}

	for (int i = 0, k = topcenterindex + 1; i < sectorcount; i++, k++)
	{
		if (i < sectorcount - 1)
		{
			indices2.push_back(topcenterindex);
			indices2.push_back(k);
			indices2.push_back(k + 1);
		}
		else
		{
			indices2.push_back(topcenterindex);
			indices2.push_back(k);
			indices2.push_back(topcenterindex + 1);
		}
	}

	GLuint real_cylinder_VAO, real_cylinder_VBO, real_cylinder_color_VBO, real_cylinder_EBO;
	glGenVertexArrays(1, &real_cylinder_VAO);
	glGenBuffers(1, &real_cylinder_VBO);
	glGenBuffers(1, &real_cylinder_color_VBO);
	glGenBuffers(1, &real_cylinder_EBO);
	
	glBindVertexArray(real_cylinder_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, real_cylinder_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(float), &vertices2[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, real_cylinder_color_VBO);
	glBufferData(GL_ARRAY_BUFFER, normals2.size() * sizeof(float), &normals2[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, real_cylinder_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices2.size() * sizeof(int), &indices2[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	return real_cylinder_VAO;
}

//画旋转圆
int DrawCircle(GLfloat x, GLfloat y, GLfloat radius, int num_segments) //xy平面
{
	float cx = x;
	float cy = y;
	float rx, ry;
	GLfloat PI = acos(-1);

	for ( int i = 0; i <= num_segments; i++)
	{
		rx = cx + radius * cos(i *  2 * PI / num_segments);
		ry = cy + radius * sin(i *  2 * PI / num_segments);
		circle_vertices.push_back(rx);
		circle_vertices.push_back(ry);
		circle_vertices.push_back(0);
		circle_indices.push_back(i);
	}

	GLuint circle_VAO, circle_VBO, circle_EBO;
	glGenVertexArrays(1, &circle_VAO);
	glGenBuffers(1, &circle_VBO);
	glGenBuffers(1, &circle_EBO);

	glBindVertexArray(circle_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, circle_VBO);
	glBufferData(GL_ARRAY_BUFFER, circle_vertices.size() * sizeof(float), &circle_vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, circle_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, circle_indices.size() * sizeof(int), &circle_indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	return circle_VAO;
}

int drawCircle(GLfloat x, GLfloat z, GLfloat radius, int num_segments) //xz平面
{
	float cx = x;
	float cz = z;
	float rx, rz;
	GLfloat PI = acos(-1);

	for (int i = 0; i <= num_segments; i++)
	{
		rx = cx + radius * cos(i * 2 * PI / num_segments);
		rz = cz + radius * sin(-i * 2 * PI / num_segments);
		circle_vertices1.push_back(rx);
		circle_vertices1.push_back(0);
		circle_vertices1.push_back(rz);
		circle_indices1.push_back(i);
	}

	GLuint circle_VAO1, circle_VBO1, circle_EBO1;
	glGenVertexArrays(1, &circle_VAO1);
	glGenBuffers(1, &circle_VBO1);
	glGenBuffers(1, &circle_EBO1);

	glBindVertexArray(circle_VAO1);
	glBindBuffer(GL_ARRAY_BUFFER, circle_VBO1);
	glBufferData(GL_ARRAY_BUFFER, circle_vertices1.size() * sizeof(float), &circle_vertices1[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, circle_EBO1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, circle_indices1.size() * sizeof(int), &circle_indices1[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	return circle_VAO1;
}

int Drawcircle(GLfloat y, GLfloat z, GLfloat radius, int num_segments) //yz平面
{
	float cy = y;
	float cz = z;
	float ry, rz;
	GLfloat PI = acos(-1);

	for (int i = 0; i <= num_segments; i++)
	{
		ry = cy + radius * cos(i * 2 * PI / num_segments);
		rz = cz + radius * sin(-i * 2 * PI / num_segments);
		circle_vertices2.push_back(0);
		circle_vertices2.push_back(ry);
		circle_vertices2.push_back(rz);
		circle_indices2.push_back(i);
	}

	GLuint circle_VAO2, circle_VBO2, circle_EBO2;
	glGenVertexArrays(1, &circle_VAO2);
	glGenBuffers(1, &circle_VBO2);
	glGenBuffers(1, &circle_EBO2);

	glBindVertexArray(circle_VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, circle_VBO2);
	glBufferData(GL_ARRAY_BUFFER, circle_vertices2.size() * sizeof(float), &circle_vertices2[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, circle_EBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, circle_indices2.size() * sizeof(int), &circle_indices2[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	return circle_VAO2;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}

}

void camera_movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);

	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);

	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);

	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);

	//Object controls
	GLfloat objectSpeed = 30.0f;
	if (keys[GLFW_KEY_UP])
		model = glm::rotate(model, glm::radians(-objectSpeed * deltaTime), object_rotate_axis);

	if (keys[GLFW_KEY_DOWN])
		model = glm::rotate(model, glm::radians(objectSpeed * deltaTime), object_rotate_axis);

	if (keys[GLFW_KEY_LEFT])
	{
		model = glm::rotate(model, glm::radians(-objectSpeed * deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
		object_rotate_axis = glm::vec3(model * glm::vec4(object_rotate_axis, 1.0f));
	}

	if (keys[GLFW_KEY_RIGHT])
	{
		model = glm::rotate(model, glm::radians(objectSpeed * deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
		object_rotate_axis = glm::vec3(model * glm::vec4(object_rotate_axis, 1.0f));
	}
}

bool mouse_flag = false; // 鼠标点击还是释放
bool initial_pickbox_flag = false;
bool end_pickbox_flag = false;
bool x_move_flag = false; // 运动还是停止
bool y_move_flag = false;
bool z_move_flag = false;
double mouseX = 0.0, mouseY = 0.0, r_mouseX = 0.0, r_mouseY = 0.0;
float temp_xpos, temp_ypos;


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		mouse_flag = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		mouse_flag = false;

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		initial_pickbox_flag = true;
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		end_pickbox_flag = true;


	if(mouse_flag == true)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		mouseX = xpos / (WIDTH * 0.5f) - 1.0f;
		mouseY = 1.0f - ypos / (HEIGHT * 0.5f) ;
		temp_xpos = mouseX;
		temp_ypos = mouseY;
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		for (int i = 0; i < circle_vertices.size()/3; i++)
		{
			glm::vec4 position = projection * view * circle_model * glm::vec4(circle_vertices[3 * i], circle_vertices[3 * i + 1], circle_vertices[3 * i + 2], 1.0f);
			position = glm::vec4(position.x / position.w, position.y / position.w, position.z / position.w, 1.0f);
			float distance = pow(mouseX - position.x, 2) + pow(mouseY - position.y, 2);
			if (distance < xy_min_distance)
				xy_min_distance = distance; //xy-plane
		}

		for (int j = 0; j < circle_vertices1.size() / 3; j++)
		{
			glm::vec4 position1 = projection * view * circle_model1 * glm::vec4(circle_vertices1[3 * j], circle_vertices1[3 * j + 1], circle_vertices1[3 * j + 2], 1.0f);
			position1 = glm::vec4(position1.x / position1.w, position1.y / position1.w, position1.z / position1.w, 1.0f);
			float distance1 = pow(mouseX - position1.x, 2) + pow(mouseY - position1.y, 2);
			if (distance1 < xz_min_distance)
				xz_min_distance = distance1; //xz-plane
		}

		for (int k = 0; k < circle_vertices2.size() / 3; k++)
		{
			glm::vec4 position2 = projection * view * circle_model2 * glm::vec4(circle_vertices2[3 * k], circle_vertices2[3 * k + 1], circle_vertices2[3 * k + 2], 1.0f);
			position2 = glm::vec4(position2.x / position2.w, position2.y / position2.w, position2.z / position2.w, 1.0f);
			float distance2 = pow(mouseX - position2.x, 2) + pow(mouseY - position2.y, 2);
			if (distance2 < yz_min_distance)
				yz_min_distance = distance2;
		}
	}
	if (mouse_flag == false)
	{
		xz_min_distance = 9999.0;
		xy_min_distance = 9999.0;
		yz_min_distance = 9999.0;
		temp_xpos = 0.0;
		temp_ypos = 0.0;
	}


	if (initial_pickbox_flag == true)
	{
		glfwGetCursorPos(window, &initial_pickbox_x, &initial_pickbox_y);
		initial_pickbox_x = initial_pickbox_x / (WIDTH * 0.5f) - 1.0f;
		initial_pickbox_y = 1.0f - initial_pickbox_y / (HEIGHT * 0.5f);
		initial_pickbox_flag = false;
	}

	if (end_pickbox_flag == true)
	{
		glfwGetCursorPos(window, &end_pickbox_x, &end_pickbox_y);
		end_pickbox_x = end_pickbox_x / (WIDTH * 0.5f) - 1.0f;
		end_pickbox_y = 1.0f - end_pickbox_y / (HEIGHT * 0.5f);
		end_pickbox_flag = false;
		selected_points_VAO = select_points(initial_pickbox_x, initial_pickbox_y, end_pickbox_x, end_pickbox_y);
	}
}

glm::vec3 self_x_axis(1.0f, 0.0f, 0.0f);
glm::vec3 self_y_axis(0.0f, 1.0f, 0.0f);
glm::vec3 self_z_axis(0.0f, 0.0f, 1.0f);

void mouse_movement(GLFWwindow* window)
{
	if (mouse_flag == true)
	{
		if (xz_min_distance < 0.0005 && xz_min_distance < xy_min_distance && xz_min_distance < yz_min_distance) //xz平面
		{
			double r_xpos, r_ypos;
			glfwGetCursorPos(window, &r_xpos, &r_ypos);
			r_mouseX = r_xpos / (WIDTH * 0.5f) - 1.0f;
			r_mouseY = 1.0f - r_ypos / (HEIGHT * 0.5f);
			float dif_x = r_mouseX - temp_xpos;
			temp_xpos = r_mouseX;
			temp_ypos = r_mouseY;
			float senstivity = 30.0;
			x_mouse_move = dif_x * senstivity;
			x_move_flag = true;
			if (r_mouseX < -1 || r_mouseX > 1)
			{
				glfwSetCursorPos(window, WIDTH/2, HEIGHT/2);
				temp_xpos = 0.0;
				temp_ypos = 0.0;
			}
		}

		else if (xy_min_distance < 0.0005 && xy_min_distance < xz_min_distance && xy_min_distance < yz_min_distance) //xy平面
		{
			double r_xpos, r_ypos;
			glfwGetCursorPos(window, &r_xpos, &r_ypos);
			r_mouseX = r_xpos / (WIDTH * 0.5f) - 1.0f;
			r_mouseY = 1.0f - r_ypos / (HEIGHT * 0.5f);
			float dif_z = r_mouseX - temp_xpos;
			temp_xpos = r_mouseX;
			temp_ypos = r_mouseY;
			float senstivity = 30.0;
			z_mouse_move = -dif_z * senstivity;
			z_move_flag = true;
			if (r_mouseX < -1 || r_mouseX >1)
			{
				glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);
				temp_xpos = 0.0;
				temp_ypos = 0.0;
			}
		}

		else if (yz_min_distance < 0.0005 && yz_min_distance < xz_min_distance && yz_min_distance < xy_min_distance) //yz平面
		{
			double r_xpos, r_ypos;
			glfwGetCursorPos(window, &r_xpos, &r_ypos);
			r_mouseX = r_xpos / (WIDTH * 0.5f) - 1.0f;
			r_mouseY = 1.0f - r_ypos / (HEIGHT * 0.5f);
			float dif_y = r_mouseX - temp_xpos;
			temp_xpos = r_mouseX;
			temp_ypos = r_mouseY;
			float senstivity = 30.0;
			y_mouse_move = -dif_y * senstivity;
			y_move_flag = true;
			if (r_mouseX < -1 || r_mouseX > 1)
			{
				glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);
				temp_xpos = 0.0;
				temp_ypos = 0.0;
			}
		}

		if (x_mouse_move != 0 && x_move_flag == true)
		{
			model = glm::rotate(model, glm::radians(x_mouse_move), self_y_axis);
			glm::mat4 temp_matrix(1.0f);
			temp_matrix = glm::rotate(temp_matrix, glm::radians(x_mouse_move), self_y_axis);
			self_x_axis = glm::vec3(temp_matrix * glm::vec4(self_x_axis, 1.0f));
			self_z_axis = glm::vec3(temp_matrix * glm::vec4(self_z_axis, 1.0f));
			circle_model = glm::rotate(circle_model, glm::radians(x_mouse_move), self_y_axis);
			circle_model2 = glm::rotate(circle_model2, glm::radians(x_mouse_move), self_y_axis);
			x_mouse_move = 0.0;
			x_move_flag = false;
		}

		if (y_mouse_move != 0 && y_move_flag == true)
		{
			model = glm::rotate(model, glm::radians(y_mouse_move), self_x_axis);
			glm::mat4 temp_matrix(1.0f);
			temp_matrix = glm::rotate(temp_matrix, glm::radians(y_mouse_move), self_x_axis);
			self_y_axis = glm::vec3(temp_matrix * glm::vec4(self_y_axis, 1.0f));
			self_z_axis = glm::vec3(temp_matrix * glm::vec4(self_z_axis, 1.0f));
			circle_model = glm::rotate(circle_model, glm::radians(y_mouse_move), self_x_axis);
			circle_model1 = glm::rotate(circle_model1, glm::radians(y_mouse_move), self_x_axis);
			y_mouse_move = 0.0;
			y_move_flag = false;
		}

		if (z_mouse_move != 0 && z_move_flag == true)
		{
			model = glm::rotate(model, glm::radians(z_mouse_move), self_z_axis);
			glm::mat4 temp_matrix(1.0f);
			temp_matrix = glm::rotate(temp_matrix, glm::radians(z_mouse_move), self_z_axis);
			self_x_axis = glm::vec3(temp_matrix * glm::vec4(self_x_axis, 1.0f));
			self_y_axis = glm::vec3(temp_matrix * glm::vec4(self_y_axis, 1.0f));
			circle_model1 = glm::rotate(circle_model1, glm::radians(z_mouse_move), self_z_axis);
			circle_model2 = glm::rotate(circle_model2, glm::radians(z_mouse_move), self_z_axis);
			z_mouse_move = 0.0;
			z_move_flag = false;
		}

	}
	
}
//void mouse_callback(GLFWwindow* window, double xpos, double ypos)
//{
//	if (firstMouse)
//	{
//		lastX = xpos;
//		lastY = ypos;
//		firstMouse = false;
//
//	}
//	GLfloat xoffset = xpos - lastX;
//	GLfloat yoffset = lastY - ypos;
//
//	lastX = xpos;
//	lastY = ypos;
//
//	camera.ProcessMouseMovement(xoffset, yoffset);
//}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

int select_points(double initial_pickbox_x, double initial_pickbox_y, double end_pickbox_x, double end_pickbox_y)
{
	selected_points.clear();
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
	for (int i = 0, k = 0; i < bunny_vertices.size() / 3; i++)
	{
		glm::vec4 position = projection * view * model * glm::vec4(bunny_vertices[3 * i], bunny_vertices[3 * i + 1], bunny_vertices[3 * i + 2], 1.0f);
		position = glm::vec4(position.x / position.w, position.y / position.w, position.z / position.w, 1.0f);
		if (initial_pickbox_x < position.x && position.x < end_pickbox_x && end_pickbox_y < position.y && position.y < initial_pickbox_y)
		{
			glm::vec3 vertex_normal = glm::vec3(bunny_normals[3 * i], bunny_normals[3 * i + 1], bunny_normals[3 * i + 2]);
			glm::vec3 view = glm::normalize(glm::vec3(camera.Position.x - bunny_vertices[3 * i], camera.Position.y - bunny_vertices[3 * i + 1], camera.Position.z - bunny_vertices[3 * i + 2]));
			if (glm::dot(vertex_normal, view) > 0)
			{
				selected_points.push_back(bunny_vertices[3 * i]);
				selected_points.push_back(bunny_vertices[3 * i + 1]);
				selected_points.push_back(bunny_vertices[3 * i + 2]);
				selected_indices.push_back(k);
				k++;
			}
		}
	}

	GLuint selected_points_VAO, selected_points_VBO, selected_points_EBO;
	glGenVertexArrays(1, &selected_points_VAO);
	glGenBuffers(1, &selected_points_VBO);
	glGenBuffers(1, &selected_points_EBO);

	glBindVertexArray(selected_points_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, selected_points_VBO);
	glBufferData(GL_ARRAY_BUFFER, selected_points.size() * sizeof(float), &selected_points[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, selected_points_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, selected_indices.size() * sizeof(int), &selected_indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
	
	return selected_points_VAO;
}