#include <stdio.h>
#include <stdlib.h>
#define GLEW_STATIC
#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<soil.h>
#include<chrono>
#include<algorithm>
#include<omp.h>

#include "Shader.h"
#include "sphere_camera.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imstb_rectpack.h"
#include "imstb_textedit.h"
#include "imstb_truetype.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "dirent.h"
#include "ImGuiFileBrowser.h"
#include "mass.h"
#include "spring.h"
#include "rope.h"

float WIDTH = 1920.0, HEIGHT = 1080.0;
Camera camera(glm::vec3(0.0f, 0.0f, 8.0f));
glm::mat4 model(1.0f);


unsigned int vertice_size = 0; // 顶点数量
unsigned int face_size = 0; //三角形数量
std::vector<GLfloat> object_vertices;
std::vector<GLfloat> object_normals;
std::vector<GLuint> object_faces;
GLuint object_VAO = 0;
GLuint circle_VAO = 0;
GLuint circle_VAO1 = 0;
GLuint circle_VAO2 = 0;
GLuint dir_circle_VAO = 0;
GLuint dir_circle_VAO1 = 0;
GLuint dir_circle_VAO2 = 0;
GLuint pickbox_VAO = 0;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
bool keys[10000];
imgui_addons::ImGuiFileBrowser file_dialog;

//object's circles
float xy_min_distance = 9999.0;
float xz_min_distance = 9999.0;
float yz_min_distance = 9999.0;
float x_mouse_move = 0.0;
float y_mouse_move = 0.0;
float z_mouse_move = 0.0;

//direcitonal light's circles
float dir_xy_min_distance = 9999.0;
float dir_xz_min_distance = 9999.0;
float dir_yz_min_distance = 9999.0;
float dir_x_mouse_move = 0.0;
float dir_y_mouse_move = 0.0;
float dir_z_mouse_move = 0.0;

glm::mat4 circle_model(1.0f);
glm::mat4 circle_model1(1.0f);
glm::mat4 circle_model2(1.0f);

glm::mat4 dir_circle_model(1.0f);
glm::mat4 dir_circle_model1(1.0f);
glm::mat4 dir_circle_model2(1.0f);


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

std::vector<float> vertices3;
std::vector<int> indices3;
std::vector<float> normals3;


//xy平面圆
std::vector<float> circle_vertices;
std::vector<int> circle_indices;

std::vector<float> dir_circle_vertices;
std::vector<int> dir_circle_indices;
//xz平面圆
std::vector<float> circle_vertices1;
std::vector<int> circle_indices1;

std::vector<float> dir_circle_vertices1;
std::vector<int> dir_circle_indices1;
//yz平面圆
std::vector<float> circle_vertices2;
std::vector<int> circle_indices2;

std::vector<float> dir_circle_vertices2;
std::vector<int> dir_circle_indices2;

GLuint planeVAO;
GLuint woodTexture;

//pickbox
bool start_pickbox = false;
std::vector<float> rectangle_vertices;
std::vector<int> rectangle_indices;
double initial_pickbox_x = 0.0, initial_pickbox_y = 0.0, end_pickbox_x = 0.0, end_pickbox_y = 0.0;
double vipb_x = 0.0, vipb_y = 0.0, vepb_x = 0.0, vepb_y = 0.0;
double temp_pickbox_x = 0.0, temp_pickbox_y = 0.0;

std::vector<int> selected_points_indices;

//bounding boxes
bool drag_object = false; // 判断是否想选中物体
std::map<int, std::vector<float>> boxes;
int pl_info, object_info;
int pl_exist;
double click_object_x, click_object_y;
double new_pos_x, new_pos_y;
bool move = false, move_pl = false, move_object = false;
double temp_pos_x = 0.0, temp_pos_y = 0.0;
glm::vec3 delta_x, delta_y;
glm::vec3 obj_delta_x, obj_delta_y;

int render_objects(std::string filename);
int draw_sphere(float radius, int sectorCount, int stackCount);
int draw_cone(float radius, float height, int sectorcount);
int draw_cylinder(float radius, float height, int sectorcount);
int draw_cylinder1(float radius, float height, int sectorcount);
int draw_xy_circle(GLfloat x, GLfloat y, GLfloat radius, int num_segments);
int draw_xz_circle(GLfloat x, GLfloat z, GLfloat radius, int num_segments);
int draw_yz_circle(GLfloat y, GLfloat z, GLfloat radius, int num_segments);
int draw_xy_circle1(GLfloat x, GLfloat y, GLfloat radius, int num_segments);
int draw_xz_circle1(GLfloat x, GLfloat z, GLfloat radius, int num_segments);
int draw_yz_circle1(GLfloat y, GLfloat z, GLfloat radius, int num_segments);
int draw_pickbox(float initial_pickbox_x, float initial_pickbox_y, float end_pickbox_x, float end_pickbox_y);
void select_points(double initial_pickbox_x, double initial_pickbox_y, double end_pickbox_x, double end_pickbox_y);


void showMainMenu();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void camera_movement();
void mouse_movement(GLFWwindow* window);
void drag_objects(GLFWwindow* window);
glm::mat4 BuildRotationMatrix(glm::vec3 start_vec, glm::vec3 end_vec);
glm::vec3 directional_light_direction = glm::vec3(3.0f, 0.0f, 1.0f);


//fast spring simulation
bool user_force = false;
bool add_force = false;
double uf_xpos = 0.0, uf_ypos = 0.0;
double start_xpos = 0.0, start_ypos = 0.0, end_xpos = 0.0, end_ypos = 0.0;
glm::vec4 arrow_position(0.0, 0.0, -1.0, 1.0);

void load_model(std::string nodefile, std::string elemfile, std::vector<glm::vec3>& model_vertices, std::vector<glm::vec3>& model_normals, std::vector<int>& model_indices, std::map<int, std::set<int>>& model_connection);
int model_vertex_size = 0;
int model_face_size = 0;

int main(int, char**)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);     
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Render", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    glewExperimental = GL_TRUE;
    glewInit();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(10.0f);
    
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    Shader bunnyshader = Shader("bunnyvertex.txt", "bunnyfragment.txt");
    Shader coneshader = Shader("cylindervertex.txt", "cylinderfragment.txt");
    Shader sphereshader = Shader("spherevertex.txt", "spherefragment.txt");
    Shader circleshader = Shader("cylindervertex.txt", "cylinderfragment.txt");
    Shader pickboxshader = Shader("cylindervertex.txt", "cylinderfragment.txt");
    Shader floorshader = Shader("floorvertex.txt", "floorfragment.txt");
    Shader mass_shader = Shader("simple_sphere_vertex.txt", "simple_sphere_fragment.txt");
    Shader cloth_shader = Shader("cloth_vertex.txt", "cloth_fragment.txt");
    Shader spring_shader = Shader("line_vertex.txt", "line_fragment.txt");
    
    // Our state
    ImVec4 background_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    glm::vec3 object_color = glm::vec3(1.0f, 0.5f, 0.31f);
    glm::vec3 directional_light_color = glm::vec3(1.0f, 1.0f, 1.0f);
    
    glm::vec3 point_color = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 point_pos = glm::vec3(-3.0f, 3.0f, 0.0f);
    int cone_VAO = draw_cone(1.0f, 2.0f, 36); //平行光
    int sphere_VAO = draw_sphere(1.0f, 100, 100); // 点光源
    int cylinder_VAO = draw_cylinder(0.5f, 2.0f, 36);
    int cylinder_VAO1 = draw_cylinder1(0.5f, 4.0f, 36);
    std::string previous_filename;
    bool directional_light = false;
    bool hide_directional_light = false;
    bool point_light = false;
    bool hide_point_light = false;
    bool pickbox = false;
    bool show_vertices = false;
    bool show_cloth = false;
    bool show_dinosaur = false;
    bool show_bar = false;

    float AS = 0.2;
    float AS1 = 0.2;
    float SS = 0.5;
    float SS1 = 0.5;
    float SN = 32.0;
    float SN1 = 32.0;

    dir_circle_model = glm::translate(dir_circle_model, glm::vec3(3.0f, 0.0f, 1.0f));
    dir_circle_model1 = glm::translate(dir_circle_model1, glm::vec3(3.0f, 0.0f, 1.0f));
    dir_circle_model2 = glm::translate(dir_circle_model2, glm::vec3(3.0f, 0.0f, 1.0f));

    dir_circle_VAO = draw_xy_circle1(0.0f, 0.0f, 0.4f, 100);
    dir_circle_VAO1 = draw_xz_circle1(0.0f, 0.0f, 0.4f, 100);
    dir_circle_VAO2 = draw_yz_circle1(0.0f, 0.0f, 0.4f, 100);


    
    glViewport(0, 0, WIDTH, HEIGHT);
    Shader shadowshader = Shader("shadow_vertex.txt", "shadow_fragment.txt");
    Shader simpleshader = Shader("simple_vertex.txt", "simple_fragment.txt");
    glUseProgram(shadowshader.ID);
    glUniform1i(glGetUniformLocation(shadowshader.ID, "diffuseTexture"), 0);
    glUniform1i(glGetUniformLocation(shadowshader.ID, "shadowMap"), 1);
    GLfloat planeVertices[] = {
        // Positions          // Normals         // Texture Coords
        25.0f, -3.5f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
        -25.0f, -3.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,
        -25.0f, -3.5f, 25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

        25.0f, -3.5f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
        25.0f, -3.5f, -25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f,
        -25.0f, -3.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f
    };

    GLuint planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glBindVertexArray(0);

    // Generate texture ID and load texture data 
    glGenTextures(1, &woodTexture);
    int width, height;
    unsigned char* image = SOIL_load_image("wood.png", &width, &height, 0, SOIL_LOAD_RGB);
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);

    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



    //mass spring system
    //cloth
    std::vector <glm::vec3> implicit_pos;
    std::vector <glm::vec3> spring_pos;
    std::vector <glm::vec3> cloth_normals;
    std::vector <int> clicked_mass_indices;
    std::vector <glm::vec3> clicked_mass_color;
    std::vector <float> clicked_mass_alpha;
    std::vector <int> cloth_pos;
    Rope* ropeVerlet = new Rope(glm::vec3(-2.0, 2.0, -2.0), glm::vec3(2.0, 2.0, 2.0), glm::vec2(20, 20), 1, 200, {380, 399}, cloth_pos);
    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    int sim_sphere_VAO = draw_sphere(1.0f, 100, 100); 
    glm::vec3 force_info(0.0f);

    //dinosaur
    std::vector<glm::vec3> dinosaur_vertices;
    std::vector<glm::vec3> dinosaur_normals;
    std::vector<int> dinosaur_indices;
    std::map<int, std::set<int>> dinosaur_connection;
    std::vector<int> dinosaur_pinned_nodes;
    std::vector<glm::vec3> dinosaur_spring_pos;
    load_model("./model/gargoyle_node.txt", "./model/gargoyle_ele.txt", dinosaur_vertices, dinosaur_normals, dinosaur_indices, dinosaur_connection);
    float x_threshold = 0.98;
    for (int i = 0; i < dinosaur_vertices.size(); i++)
    {
        if (dinosaur_vertices[i].x > x_threshold)
            dinosaur_pinned_nodes.push_back(i);
    }
    //Rope* dinosaur_ani = new Rope(1, 500, dinosaur_vertices, dinosaur_connection, dinosaur_pinned_nodes);

    Rope* Newton_ani = new Rope(1, 500, dinosaur_vertices, dinosaur_connection, dinosaur_pinned_nodes, 1);

    ////bar
    //std::vector<glm::vec3> bar_vertices;
    //std::vector<glm::vec3> bar_normals;
    //std::vector<int> bar_indices;
    //std::map<int, std::set<int>> bar_connection;
    //load_model("./model/bar_node.txt", "./model/bar_ele.txt", bar_vertices, bar_normals, bar_indices, bar_connection);
    //std::vector<int> bar_pinned_nodes;

    //float x_min = 999.0;
    //for (int i = 0; i < bar_vertices.size(); i++)
    //{
    //    if (bar_vertices[i].x < x_min)
    //    {
    //        x_min = bar_vertices[i].x;
    //        bar_pinned_nodes.push_back(i);
    //    }

    //    else if (bar_vertices[i].x == x_min)
    //        bar_pinned_nodes.push_back(i);
    //}

    //Rope* bar_ani = new Rope(1, 100, bar_vertices, bar_connection, bar_pinned_nodes);
    //std::vector<glm::vec3> bar_spring_pos;

    
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        camera_movement();
        mouse_movement(window);
        drag_objects(window);
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

        glm::vec3 horizontal_dir = camera.GetRightVector();
        glm::vec3 vertical_dir = camera.GetUpVector();
        // Main Menu
        {
            bool my_tool_active = true;
            ImGui::Begin("Main Menu.", &my_tool_active, ImGuiWindowFlags_MenuBar);

            // Load objects
            ImGui::Text("Select your file to render by clicking the menu button");
            showMainMenu();
            if (!file_dialog.selected_path.empty() && previous_filename != file_dialog.selected_path)
            {
                object_VAO = render_objects(file_dialog.selected_path);
                circle_VAO = draw_xy_circle(0.0f, 0.0f, 1.5f, 100);
                circle_VAO1 = draw_xz_circle(0.0f, 0.0f, 1.5f, 100);
                circle_VAO2 = draw_yz_circle(0.0f, 0.0f, 1.5f, 100);
                previous_filename = file_dialog.selected_path;
                file_dialog.selected_path.clear();
            }

            // Select background color;
            ImGui::Text("Select the color of background");
            ImGui::ColorEdit3("color of background", (float*)&background_color);

            // Selete object color;
            ImGui::Text("Select the color of object");
            ImGui::ColorEdit3("color of object", (float*)&object_color);


            ImGui::Text("Button to show vertices");
            if (ImGui::Button("show vertices"))
                show_vertices = true;
            ImGui::SameLine();
            if (ImGui::Button("hide vertices"))
                show_vertices = false;

            ImGui::Text("Button to show cloth animation");
            if (ImGui::Button("show cloth"))
                show_cloth = true;
            ImGui::SameLine();
            if (ImGui::Button("hide cloth"))
                show_cloth = false;

            ImGui::Text("Button to show bunny animation");
            if (ImGui::Button("show dinosaur"))
                show_dinosaur = true;
            ImGui::SameLine();
            if (ImGui::Button("hide dinosaur"))
                show_dinosaur = false;

            ImGui::Text("Button to show bar animation");
            if (ImGui::Button("show bar"))
                show_bar = true;
            ImGui::SameLine();
            if (ImGui::Button("hide bar"))
                show_bar = false;

            ImGui::End();

        }
        
        // Light Menu
        {
            ImGui::Begin("Light Menu");

            ImGui::Text("Directional light");
            ImGui::SameLine();
            if (ImGui::Button("Open"))
                directional_light = true;
            ImGui::SameLine();
            if (ImGui::Button("Close"))
                directional_light = false;
            ImGui::SameLine();
            if (ImGui::Button("Hide"))
                hide_directional_light = true;
            ImGui::SameLine();
            if (ImGui::Button("Show"))
                hide_directional_light = false;

            while (directional_light)
            {


                // Set color of directional light
                ImGui::Text("Set the color of directional light");
                ImGui::ColorEdit3("color of directional light", (float*)&directional_light_color);

                // Set direction of directional light
                ImGui::Text("Set the direciton of directional light");
                ImGui::InputFloat3("direciton of directional light", (float*)&directional_light_direction);
                
                ImGui::Text("Set the parameters");
                ImGui::InputFloat("ambientstrength", (float*)&AS1);
                ImGui::InputFloat("specularstrength", (float*)&SS1);
                ImGui::InputFloat("shineness", (float*)&SN1);
                break;
            }

            ImGui::Text("Point light");
            ImGui::SameLine();
            if (ImGui::Button("open"))
            {
                point_light = true;
                pl_exist = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("close"))
            {
                point_light = false;
                pl_exist = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("hide"))
                hide_point_light = true;
            ImGui::SameLine();
            if (ImGui::Button("show"))
                hide_point_light = false;

            while(point_light)
            {
                //Set color of point light
                ImGui::Text("Set the color of point light");
                ImGui::ColorEdit3("color of point light " , (float*)&point_color);

                //Set position of point light
                ImGui::Text("Set the position of point light");
                ImGui::InputFloat3("position of point light", (float*)&point_pos);

                ImGui::Text("set the parameters");
                ImGui::InputFloat("AmbientStrength", (float*)&AS);
                ImGui::InputFloat("SpecularStrength", (float*)&SS);
                ImGui::InputFloat("Shineness", (float*)&SN);
                break;
            }
            ImGui::End();
        }

        //Force Menu
        {
            ImGui::Begin("User force Menu");
            ImGui::Text("Force Direction");
            ImGui::Text("Unit Force in x direction: %f", force_info.x);
            ImGui::Text("Unit Force in y direction: %f", force_info.y);
            ImGui::Text("Unit Force in z direction: %f", force_info.z);
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        glViewport(0, 0, WIDTH, HEIGHT);
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(background_color.x, background_color.y, background_color.z, background_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //draw pickbox simutaneously
        if (start_pickbox == true)
        {
            glfwGetCursorPos(window, &temp_pickbox_x, &temp_pickbox_y);
            temp_pickbox_y = HEIGHT - temp_pickbox_y;
            pickbox_VAO = draw_pickbox(initial_pickbox_x, initial_pickbox_y, temp_pickbox_x, temp_pickbox_y);
            
        }
        else
        {
            temp_pickbox_x = 0.0;
            temp_pickbox_y = 0.0;
            pickbox_VAO = 0;
        }


        // render object
        if (object_VAO != 0 && show_vertices == false)
        {

            glm::mat4 lightProjection, lightView;
            glm::mat4 lightSpaceMatrix;
            GLfloat near_plane = 0.1f, far_plane = 100.0f;
            lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
            lightView = glm::lookAt(point_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));
            lightSpaceMatrix = lightProjection * lightView;
            glUseProgram(simpleshader.ID);
            glUniformMatrix4fv(glGetUniformLocation(simpleshader.ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            glCullFace(GL_FRONT);
            glm::mat4 floor_model(1.0f);
            glUniformMatrix4fv(glGetUniformLocation(simpleshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(floor_model));
            glBindVertexArray(planeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);



            
            glUniformMatrix4fv(glGetUniformLocation(simpleshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glBindVertexArray(object_VAO);
            glDrawElements(GL_TRIANGLES, object_faces.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            glCullFace(GL_BACK);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glViewport(0, 0, WIDTH, HEIGHT);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glUseProgram(shadowshader.ID);
            glUniformMatrix4fv(glGetUniformLocation(shadowshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(glGetUniformLocation(shadowshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniform3f(glGetUniformLocation(shadowshader.ID, "lightPos"), point_pos.x, point_pos.y, point_pos.z);
            glUniform3f(glGetUniformLocation(shadowshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            glUniformMatrix4fv(glGetUniformLocation(shadowshader.ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, woodTexture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, depthMap);

            glUniformMatrix4fv(glGetUniformLocation(shadowshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(floor_model));
            glBindVertexArray(planeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);

            
            glUseProgram(bunnyshader.ID);
            model = glm::translate(model, obj_delta_x + obj_delta_y);
            glUniformMatrix4fv(glGetUniformLocation(bunnyshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(glGetUniformLocation(bunnyshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(bunnyshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform3f(glGetUniformLocation(bunnyshader.ID, "objectColor"), object_color.x, object_color.y, object_color.z);
            glUniform3f(glGetUniformLocation(bunnyshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            if (point_light)
            {
                glUniform3f(glGetUniformLocation(bunnyshader.ID, "lightColor"), point_color.x, point_color.y, point_color.z);
                glUniform3f(glGetUniformLocation(bunnyshader.ID, "lightPos"), point_pos.x, point_pos.y, point_pos.z);
                glUniform1f(glGetUniformLocation(bunnyshader.ID, "AS"), AS);
                glUniform1f(glGetUniformLocation(bunnyshader.ID, "SS"), SS);
                glUniform1f(glGetUniformLocation(bunnyshader.ID, "SN"), SN);
            }
            else
            {
                glUniform3f(glGetUniformLocation(bunnyshader.ID, "lightColor"), 0.0f, 0.0f, 0.0f);
                glUniform3f(glGetUniformLocation(bunnyshader.ID, "lightPos"), 0.0f, 0.0f, 0.0f);
            }
            if (directional_light)
            {
                glUniform3f(glGetUniformLocation(bunnyshader.ID, "direColor"), directional_light_color.x, directional_light_color.y, directional_light_color.z);
                glUniform3f(glGetUniformLocation(bunnyshader.ID, "direDir"), directional_light_direction.x, directional_light_direction.y, directional_light_direction.z);
                glUniform1f(glGetUniformLocation(bunnyshader.ID, "AS1"), AS1);
                glUniform1f(glGetUniformLocation(bunnyshader.ID, "SS1"), SS1);
                glUniform1f(glGetUniformLocation(bunnyshader.ID, "SN1"), SN1);
            }
            else
            {
                glUniform3f(glGetUniformLocation(bunnyshader.ID, "direColor"), 0.0f, 0.0f, 0.0f);
                glUniform3f(glGetUniformLocation(bunnyshader.ID, "direDir"), 0.0f, 0.0f, 0.0f);
            }
            glBindVertexArray(object_VAO);
            glDrawElements(GL_TRIANGLES, object_faces.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            object_info = object_VAO;
            glm::vec4 temp_coord = glm::vec4(projection * view * model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
            glm::vec3 screen_coord = glm::vec3(temp_coord.x / temp_coord.w, temp_coord.y / temp_coord.w, temp_coord.z / temp_coord.w);
            boxes[object_info].clear();
            boxes[object_info].push_back(screen_coord.x);
            boxes[object_info].push_back(screen_coord.y);
            boxes[object_info].push_back(screen_coord.z);
            glm::vec4 bbox_coord = glm::vec4(projection * view * model * glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
            float width = abs(bbox_coord.x / bbox_coord.w - screen_coord.x);
            float height = abs(bbox_coord.y / bbox_coord.w - screen_coord.y);
            boxes[object_info].push_back(width);
            boxes[object_info].push_back(height);


        }


        //show vertices of object
        if (object_VAO != 0 && show_vertices == true)
        {
            //glUseProgram(sphereshader.ID);
            //glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            //glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            //glUniform3f(glGetUniformLocation(sphereshader.ID, "objectColor"), point_color.x, point_color.y, point_color.z);
            //glUniform3f(glGetUniformLocation(sphereshader.ID, "lightPos"), 0.0f, 0.0f, 10.0f);
            //glUniform3f(glGetUniformLocation(sphereshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            //glBindVertexArray(0);
            //for (int i = 0; i < object_vertices.size() / 3; i++)
            //{
            //    if (selected_points_indices.size() != 0)
            //    {
            //        std::vector<int>::iterator it = std::find(selected_points_indices.begin(), selected_points_indices.end(), i);
            //        if (it != selected_points_indices.end())
            //        {
            //            glm::mat4 vertex_sphere_model(1.0f);
            //            vertex_sphere_model = glm::translate(vertex_sphere_model, glm::vec3(object_vertices[i * 3], object_vertices[i * 3 + 1], object_vertices[i * 3 + 2]));
            //            vertex_sphere_model = glm::scale(vertex_sphere_model, glm::vec3(0.002f, 0.002f, 0.002f));
            //            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(vertex_sphere_model));
            //            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), 1.0f, 0.0f, 0.0f);
            //            glBindVertexArray(sphere_VAO);
            //            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            //            glBindVertexArray(0);
            //        }
            //        else
            //        {
            //            glm::mat4 vertex_sphere_model(1.0f);
            //            vertex_sphere_model = glm::translate(vertex_sphere_model, glm::vec3(object_vertices[i * 3], object_vertices[i * 3 + 1], object_vertices[i * 3 + 2]));
            //            vertex_sphere_model = glm::scale(vertex_sphere_model, glm::vec3(0.002f, 0.002f, 0.002f));
            //            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(vertex_sphere_model));
            //            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), 1.0f, 1.0f, 1.0f);
            //            glBindVertexArray(sphere_VAO);
            //            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            //            glBindVertexArray(0);
            //        }
            //    }
            //    else
            //    {
            //        glm::mat4 vertex_sphere_model(1.0f);
            //        vertex_sphere_model = glm::translate(vertex_sphere_model, glm::vec3(object_vertices[i * 3], object_vertices[i * 3 + 1], object_vertices[i * 3 + 2]));
            //        vertex_sphere_model = glm::scale(vertex_sphere_model, glm::vec3(0.002f, 0.002f, 0.002f));
            //        glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(vertex_sphere_model));
            //        glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), 1.0f, 1.0f, 1.0f);
            //        glBindVertexArray(sphere_VAO);
            //        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            //        glBindVertexArray(0);
            //    }
            //}

            //glUseProgram(verticesshader.ID);
            //glm::mat4 vertices_model(1.0f);
            //glUniformMatrix4fv(glGetUniformLocation(verticesshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(vertices_model));
            //glUniformMatrix4fv(glGetUniformLocation(verticesshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            //glUniformMatrix4fv(glGetUniformLocation(verticesshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            //glUniform1f(glGetUniformLocation(verticesshader.ID, "radius"), 1.0);
            //glUniform3f(glGetUniformLocation(verticesshader.ID, "center"), 0.0, 0.0, 0.0);
            //GLuint vertices_indices[6] = { 0, 1, 2, 1, 2, 3 };
            //GLuint vertices_VAO, vertices_EBO;
            //glGenVertexArrays(1, &vertices_VAO);
            //glGenBuffers(1, &vertices_EBO);
            //glBindVertexArray(vertices_VAO);
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertices_EBO);
            //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertices_indices), &vertices_indices[0], GL_STATIC_DRAW);
            //glDrawElements(GL_TRIANGLES, sizeof(vertices_indices), GL_UNSIGNED_INT, (GLvoid*)0);
            //glBindVertexArray(0);
            
        }

        //render point light
        if (point_light == true && hide_point_light == false)
        {
            glUseProgram(sphereshader.ID);
            glm::mat4 pl_model(1.0f);
            point_pos = point_pos + delta_x + delta_y;
            pl_model = glm::translate(pl_model, point_pos);
            pl_model = glm::scale(pl_model, glm::vec3(0.1f, 0.1f, 0.1f));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(pl_model));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), 1.0f, 1.0f, 1.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "objectColor"), point_color.x, point_color.y, point_color.z);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightPos"), 0.0f, 0.0f, 10.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            glBindVertexArray(sphere_VAO);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            //bounding box of point light
            pl_info = sphere_VAO;
            glm::vec4 temp_coord = glm::vec4(projection * view * pl_model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
            glm::vec3 screen_coord = glm::vec3(temp_coord.x / temp_coord.w, temp_coord.y / temp_coord.w, temp_coord.z / temp_coord.w);
            boxes[pl_info].clear();
            boxes[pl_info].push_back(screen_coord.x);
            boxes[pl_info].push_back(screen_coord.y);
            boxes[pl_info].push_back(screen_coord.z);
            glm::vec4 bbox_coord = glm::vec4(projection * view * pl_model * glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
            float width = abs(bbox_coord.x / bbox_coord.w - screen_coord.x);
            float height = abs(bbox_coord.y / bbox_coord.w - screen_coord.y);
            boxes[pl_info].push_back(width);
            boxes[pl_info].push_back(height);
        }
        

        //render direcitonal light 
        if (directional_light == true && hide_directional_light == false)
        {
            glm::vec3 temp_direction = glm::normalize(directional_light_direction);
            float angle_t = atan(temp_direction.z / temp_direction.x);
            float angle_p = acos(temp_direction.y);
            glUseProgram(sphereshader.ID);
            glm::mat4 cone_model_1(1.0f);
            cone_model_1 = glm::translate(cone_model_1, glm::vec3(2.0f, 1.0f, 1.0f));
            cone_model_1 = glm::rotate(cone_model_1, -angle_t, glm::vec3(0.0f, 1.0f, 0.0f));
            cone_model_1 = glm::rotate(cone_model_1, angle_p, glm::vec3(0.0f, 0.0f, 1.0f));
            cone_model_1 = glm::scale(cone_model_1, glm::vec3(0.1f, 0.1f, 0.1f));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(cone_model_1));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), directional_light_color.x, directional_light_color.y, directional_light_color.z);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightPos"), -5.0f, -5.0f, -5.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "objectColor"), 66.0 / 255.0f, 165.0 / 255.0f, 159.0 / 255.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            glBindVertexArray(cone_VAO);
            glDrawElements(GL_TRIANGLES, indices1.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);


            glm::mat4 cone_model_2(1.0f);
            cone_model_2 = glm::translate(cone_model_2, glm::vec3(2.0f, 0.0f, 1.0f));
            cone_model_2 = glm::rotate(cone_model_2, -angle_t, glm::vec3(0.0f, 1.0f, 0.0f));
            cone_model_2 = glm::rotate(cone_model_2, angle_p, glm::vec3(0.0f, 0.0f, 1.0f));
            cone_model_2 = glm::scale(cone_model_2, glm::vec3(0.1f, 0.1f, 0.1f));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(cone_model_2));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), directional_light_color.x, directional_light_color.y, directional_light_color.z);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightPos"), -5.0f, -5.0f, -5.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            glBindVertexArray(cone_VAO);
            glDrawElements(GL_TRIANGLES, indices1.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);


            glm::mat4 cone_model_3(1.0f);
            cone_model_3 = glm::translate(cone_model_3, glm::vec3(2.0f, -1.0f, 1.0f));
            cone_model_3 = glm::rotate(cone_model_3, -angle_t, glm::vec3(0.0f, 1.0f, 0.0f));
            cone_model_3 = glm::rotate(cone_model_3, angle_p, glm::vec3(0.0f, 0.0f, 1.0f));
            cone_model_3 = glm::scale(cone_model_3, glm::vec3(0.1f, 0.1f, 0.1f));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(cone_model_3));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), directional_light_color.x, directional_light_color.y, directional_light_color.z);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightPos"), -5.0f, -5.0f, -5.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            glBindVertexArray(cone_VAO);
            glDrawElements(GL_TRIANGLES, indices1.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            glm::mat4 cylinder_model_1(1.0f);
            cylinder_model_1 = glm::translate(cylinder_model_1, glm::vec3(2.0f, 1.0f, 1.0f));
            cylinder_model_1 = glm::rotate(cylinder_model_1, -angle_t, glm::vec3(0.0f, 1.0f, 0.0f));
            cylinder_model_1 = glm::rotate(cylinder_model_1, angle_p, glm::vec3(0.0f, 0.0f, 1.0f));
            cylinder_model_1 = glm::scale(cylinder_model_1, glm::vec3(0.1f, 0.1f, 0.1f));
            cylinder_model_1 = glm::translate(cylinder_model_1, glm::vec3(0.0f, -2.0f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(cylinder_model_1));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), directional_light_color.x, directional_light_color.y, directional_light_color.z);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightPos"), -5.0f, -5.0f, -5.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            glBindVertexArray(cylinder_VAO1);
            glDrawElements(GL_TRIANGLES, indices3.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);


            glm::mat4 cylinder_model_2(1.0f);
            cylinder_model_2 = glm::translate(cylinder_model_2, glm::vec3(2.0f, 0.0f, 1.0f));
            cylinder_model_2 = glm::rotate(cylinder_model_2, -angle_t, glm::vec3(0.0f, 1.0f, 0.0f));
            cylinder_model_2 = glm::rotate(cylinder_model_2, angle_p, glm::vec3(0.0f, 0.0f, 1.0f));
            cylinder_model_2 = glm::scale(cylinder_model_2, glm::vec3(0.1f, 0.1f, 0.1f));
            cylinder_model_2 = glm::translate(cylinder_model_2, glm::vec3(0.0f, -2.0f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(cylinder_model_2));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), directional_light_color.x, directional_light_color.y, directional_light_color.z);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightPos"), -5.0f, -5.0f, -5.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            glBindVertexArray(cylinder_VAO1);
            glDrawElements(GL_TRIANGLES, indices3.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);


            glm::mat4 cylinder_model_3(1.0f);
            cylinder_model_3 = glm::translate(cylinder_model_3, glm::vec3(2.0f, -1.0f, 1.0f));
            cylinder_model_3 = glm::rotate(cylinder_model_3, -angle_t, glm::vec3(0.0f, 1.0f, 0.0f));
            cylinder_model_3 = glm::rotate(cylinder_model_3, angle_p, glm::vec3(0.0f, 0.0f, 1.0f));
            cylinder_model_3 = glm::scale(cylinder_model_3, glm::vec3(0.1f, 0.1f, 0.1f));
            cylinder_model_3 = glm::translate(cylinder_model_3, glm::vec3(0.0f, -2.0f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(cylinder_model_3));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), directional_light_color.x, directional_light_color.y, directional_light_color.z);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightPos"), -5.0f, -5.0f, -5.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            glBindVertexArray(cylinder_VAO1);
            glDrawElements(GL_TRIANGLES, indices3.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);


            glUseProgram(circleshader.ID);
            glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(dir_circle_model));
            glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            if (dir_xy_min_distance < 0.0005 && dir_xy_min_distance < dir_xz_min_distance && dir_xy_min_distance < dir_yz_min_distance)
                glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 0.0f, 0.0f, 1.0f);
            else
                glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 0.0f, 0.0f, 0.3f);
            glBindVertexArray(dir_circle_VAO);
            glDrawElements(GL_LINE_LOOP, dir_circle_indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);



            glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(dir_circle_model1));
            glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            if (dir_xz_min_distance < 0.0005 && dir_xz_min_distance < dir_xy_min_distance && dir_xz_min_distance < dir_yz_min_distance)
                glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 1.0f, 0.0f, 0.0f);
            else
                glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 0.3f, 0.0f, 0.0f);
            glBindVertexArray(dir_circle_VAO1);
            glDrawElements(GL_LINE_LOOP, dir_circle_indices1.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);



            glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(dir_circle_model2));
            glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            if (dir_yz_min_distance < 0.0005 && dir_yz_min_distance < dir_xz_min_distance && dir_yz_min_distance < dir_xy_min_distance)
                glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 0.0f, 1.0f, 0.0f);
            else
                glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 0.0f, 0.3f, 0.0f);
            glBindVertexArray(dir_circle_VAO2);
            glDrawElements(GL_LINE_LOOP, dir_circle_indices2.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        // render rotation cirlce
        if (object_VAO != 0)
        {
            glUseProgram(circleshader.ID);
            circle_model = glm::translate(circle_model, obj_delta_x + obj_delta_y);
            glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(circle_model));
            glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            if (xy_min_distance < 0.0005 && xy_min_distance < xz_min_distance && xy_min_distance < yz_min_distance)
                glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 0.0f, 0.0f, 1.0f);
            else
                glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 0.0f, 0.0f, 0.5f);
            glBindVertexArray(circle_VAO);
            glDrawElements(GL_LINE_LOOP, circle_indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            circle_model1 = glm::translate(circle_model1, obj_delta_x + obj_delta_y);
            glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(circle_model1));
            glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            if (xz_min_distance < 0.0005 && xz_min_distance < xy_min_distance && xz_min_distance < yz_min_distance)
                glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 1.0f, 0.0f, 0.0f);
            else
                glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 0.5f, 0.0f, 0.0f);
            glBindVertexArray(circle_VAO1);
            glDrawElements(GL_LINE_LOOP, circle_indices1.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            circle_model2 = glm::translate(circle_model2, obj_delta_x + obj_delta_y);
            glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(circle_model2));
            glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            if (yz_min_distance < 0.0005 && yz_min_distance < xz_min_distance && yz_min_distance < xy_min_distance)
                glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 0.0f, 1.0f, 0.0f);
            else
                glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 0.0f, 0.5f, 0.0f);
            glBindVertexArray(circle_VAO2);
            glDrawElements(GL_LINE_LOOP, circle_indices2.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        //render pickbox
        if (pickbox_VAO != 0)
        {
            glUseProgram(pickboxshader.ID);
            glm::mat4 pickbox_model(1.0f);
            glm::mat4 pickbox_view = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 pickbox_projection = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, 0.1f, 100.0f);
            glUniformMatrix4fv(glGetUniformLocation(pickboxshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(pickbox_model));
            glUniformMatrix4fv(glGetUniformLocation(pickboxshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(pickbox_view));
            glUniformMatrix4fv(glGetUniformLocation(pickboxshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(pickbox_projection));
            glUniform3f(glGetUniformLocation(pickboxshader.ID, "lightColor"), 1.0f, 1.0f, 1.0f);
            glBindVertexArray(pickbox_VAO);
            glDrawElements(GL_LINE_LOOP, rectangle_indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);


        }

        
        if (show_cloth == true)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            implicit_pos.clear();
            spring_pos.clear();
            cloth_normals.clear();
            glm::mat4 MVP = projection * view * model;
            glm::mat4 inverse_MVP = glm::inverse(MVP);   
            Vector3d f_user(0.0, 0.0, 0.0);
            glm::vec2 clicked_pos;
            

            glm::vec3 uf(0.0f);
            if (user_force == false && add_force == false)
            {
                if (start_xpos != end_xpos || start_ypos != end_ypos)
                {
                    double screen_x_dif = end_xpos - start_xpos;
                    double screen_y_dif = end_ypos - start_ypos;
                    uf = glm::vec3(horizontal_dir.x * screen_x_dif + vertical_dir.x * screen_y_dif, horizontal_dir.y * screen_x_dif + vertical_dir.y * screen_y_dif, horizontal_dir.z * screen_x_dif + vertical_dir.z * screen_y_dif);
                    uf = glm::normalize(uf);
                    
                    f_user(0) = uf.x;
                    f_user(1) = uf.y;
                    f_user(2) = uf.z;
                    force_info = uf;
                    clicked_pos.x = start_xpos;
                    clicked_pos.y = start_ypos;
                    start_xpos = 0;
                    start_ypos = 0;
                    end_xpos = 0;
                    end_ypos = 0;

                }
            }
            //auto t1 = high_resolution_clock::now();
            ropeVerlet->implicitEuler(0.02, glm::vec3(0.0, -0.1, 0.0), implicit_pos, spring_pos, cloth_normals, clicked_mass_indices, clicked_mass_alpha, clicked_mass_color, clicked_pos, f_user, view, projection);
             //ropeVerlet->implicitEuler(0.02, glm::vec3(0.0, -0.1, 0.0), implicit_pos, spring_pos, bunny_vertices);
            //auto t2 = high_resolution_clock::now();
            //duration <double, std::milli> time = t2 - t1;
            //std::cout << time.count() << "ms\n";
            
            //GLuint simple_sphere_indices[6] = { 0, 1, 2, 1, 2, 3 };
            //GLuint simple_sphere_VAO, simple_sphere_EBO;
            //glGenVertexArrays(1, &simple_sphere_VAO);
            //glGenBuffers(1, &simple_sphere_EBO);
            //glBindVertexArray(simple_sphere_VAO);
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, simple_sphere_EBO);
            //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(simple_sphere_indices), &simple_sphere_indices[0], GL_STATIC_DRAW);
            //glBindVertexArray(0);

            //glUseProgram(mass_shader.ID);
            //glm::mat4 mass_model(1.0f);
            //glUniformMatrix4fv(glGetUniformLocation(mass_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(mass_model));
            //glUniformMatrix4fv(glGetUniformLocation(mass_shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            //glUniformMatrix4fv(glGetUniformLocation(mass_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            //glUniform1f(glGetUniformLocation(mass_shader.ID, "radius"), 0.2);
            //for (int i = 0; i < clicked_mass_pos.size(); i++)
            //{
            //    glUniform3f(glGetUniformLocation(mass_shader.ID, "center"), clicked_mass_pos[i].x, clicked_mass_pos[i].y, clicked_mass_pos[i].z);
            //    glBindVertexArray(simple_sphere_VAO);
            //    glDrawElements(GL_TRIANGLES, sizeof(simple_sphere_indices), GL_UNSIGNED_INT, 0);
            //    glBindVertexArray(0);
            //}

            //GLuint spring_VAO, spring_VBO;
            //glGenVertexArrays(1, &spring_VAO);
            //glGenBuffers(1, &spring_VBO);
            //glBindVertexArray(spring_VAO);
            //glBindBuffer(GL_ARRAY_BUFFER, spring_VBO);
            //glBufferData(GL_ARRAY_BUFFER, spring_pos.size() * sizeof(glm::vec3), &spring_pos[0], GL_STATIC_DRAW);
            //glEnableVertexAttribArray(0);
            //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
            //glBindVertexArray(0);

            //glUseProgram(spring_shader.ID);
            //glm::mat4 spring_model(1.0f);
            //glUniformMatrix4fv(glGetUniformLocation(spring_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(spring_model));
            //glUniformMatrix4fv(glGetUniformLocation(spring_shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            //glUniformMatrix4fv(glGetUniformLocation(spring_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            //glUniform3f(glGetUniformLocation(spring_shader.ID, "object_color"), 1.0f, 0.0f, 0.0f);
            //glBindVertexArray(spring_VAO);
            //glDrawArrays(GL_LINES, 0, spring_pos.size());
            //glBindVertexArray(0);


            //cloth
            glm::mat4 cloth_model(1.0f);
            glm::mat4 sim_sphere_model(1.0f);

            GLuint cloth_VAO, cloth_VBO, cloth_nVBO, cloth_EBO, cmAlpha_VBO, cmColor_VBO;
            glGenVertexArrays(1, &cloth_VAO);
            glGenBuffers(1, &cloth_VBO);
            glGenBuffers(1, &cloth_nVBO);
            glGenBuffers(1, &cloth_EBO);
            glGenBuffers(1, &cmAlpha_VBO);
            glGenBuffers(1, &cmColor_VBO);
            glBindVertexArray(cloth_VAO);
            glBindBuffer(GL_ARRAY_BUFFER, cloth_VBO);
            glBufferData(GL_ARRAY_BUFFER, implicit_pos.size() * sizeof(glm::vec3), &implicit_pos[0], GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, cloth_nVBO);
            glBufferData(GL_ARRAY_BUFFER, cloth_normals.size() * sizeof(glm::vec3), &cloth_normals[0], GL_STATIC_DRAW);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, cmColor_VBO);
            glBufferData(GL_ARRAY_BUFFER, clicked_mass_color.size() * sizeof(glm::vec3), &clicked_mass_color[0], GL_STATIC_DRAW);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, cmAlpha_VBO);
            glBufferData(GL_ARRAY_BUFFER, clicked_mass_alpha.size() * sizeof(float), &clicked_mass_alpha[0], GL_STATIC_DRAW);
            glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
            glEnableVertexAttribArray(3);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cloth_EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, cloth_pos.size() * sizeof(int), &cloth_pos[0], GL_STATIC_DRAW);
            glBindVertexArray(0);


            glm::mat4 lightProjection, lightView;
            glm::mat4 lightSpaceMatrix;
            GLfloat near_plane = 0.1f, far_plane = 100.0f;
            lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
            lightView = glm::lookAt(point_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));
            lightSpaceMatrix = lightProjection * lightView;
            glUseProgram(simpleshader.ID);
            glUniformMatrix4fv(glGetUniformLocation(simpleshader.ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            glCullFace(GL_FRONT);
            glm::mat4 floor_model(1.0f);
            glUniformMatrix4fv(glGetUniformLocation(simpleshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(floor_model));
            glBindVertexArray(planeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);

            glUniformMatrix4fv(glGetUniformLocation(simpleshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(cloth_model));
            glBindVertexArray(cloth_VAO);
            glDrawElements(GL_TRIANGLES, cloth_pos.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            glUniformMatrix4fv(glGetUniformLocation(simpleshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(sim_sphere_model));
            glBindVertexArray(sim_sphere_VAO);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            glCullFace(GL_BACK);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glViewport(0, 0, WIDTH, HEIGHT);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glUseProgram(shadowshader.ID);
            glUniformMatrix4fv(glGetUniformLocation(shadowshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(glGetUniformLocation(shadowshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniform3f(glGetUniformLocation(shadowshader.ID, "lightPos"), point_pos.x, point_pos.y, point_pos.z);
            glUniform3f(glGetUniformLocation(shadowshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            glUniformMatrix4fv(glGetUniformLocation(shadowshader.ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, woodTexture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            glUniformMatrix4fv(glGetUniformLocation(shadowshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(floor_model));
            glBindVertexArray(planeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);

            glUseProgram(cloth_shader.ID);
            glUniformMatrix4fv(glGetUniformLocation(cloth_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(cloth_model));
            glUniformMatrix4fv(glGetUniformLocation(cloth_shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(cloth_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            //glUniform3f(glGetUniformLocation(cloth_shader.ID, "lightColor"), point_color.x, point_color.y, point_color.z);
            //glUniform3f(glGetUniformLocation(cloth_shader.ID, "lightPos"), point_pos.x, point_pos.y, point_pos.z);
            glUniform3f(glGetUniformLocation(cloth_shader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            if (point_light)
            {
                glUniform3f(glGetUniformLocation(cloth_shader.ID, "lightColor"), point_color.x, point_color.y, point_color.z);
                glUniform3f(glGetUniformLocation(cloth_shader.ID, "lightPos"), point_pos.x, point_pos.y, point_pos.z);
                glUniform1f(glGetUniformLocation(cloth_shader.ID, "AS"), AS);
                glUniform1f(glGetUniformLocation(cloth_shader.ID, "SS"), SS);
                glUniform1f(glGetUniformLocation(cloth_shader.ID, "SN"), SN);
            }
            else
            {
                glUniform3f(glGetUniformLocation(cloth_shader.ID, "lightColor"), 0.0f, 0.0f, 0.0f);
                glUniform3f(glGetUniformLocation(cloth_shader.ID, "lightPos"), 0.0f, 0.0f, 0.0f);
            }
            if (directional_light)
            {
                glUniform3f(glGetUniformLocation(cloth_shader.ID, "direColor"), directional_light_color.x, directional_light_color.y, directional_light_color.z);
                glUniform3f(glGetUniformLocation(cloth_shader.ID, "direDir"), directional_light_direction.x, directional_light_direction.y, directional_light_direction.z);
                glUniform1f(glGetUniformLocation(cloth_shader.ID, "AS1"), AS1);
                glUniform1f(glGetUniformLocation(cloth_shader.ID, "SS1"), SS1);
                glUniform1f(glGetUniformLocation(cloth_shader.ID, "SN1"), SN1);
            }
            else
            {
                glUniform3f(glGetUniformLocation(cloth_shader.ID, "direColor"), 0.0f, 0.0f, 0.0f);
                glUniform3f(glGetUniformLocation(cloth_shader.ID, "direDir"), 0.0f, 0.0f, 0.0f);
            }
            glBindVertexArray(cloth_VAO);
            glDrawElements(GL_TRIANGLES, cloth_pos.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);


            glUseProgram(sphereshader.ID);
            sim_sphere_model = glm::translate(sim_sphere_model, glm::vec3(0.75f, 0.0f, 0.0f));
            sim_sphere_model = glm::scale(sim_sphere_model, glm::vec3(0.95f, 0.95f, 0.95f));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(sim_sphere_model));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), point_color.x, point_color.y, point_color.z);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "objectColor"), 0.3f, 0.4f, 0.5f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightPos"), point_pos.x, point_pos.y, point_pos.z);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            glBindVertexArray(sim_sphere_VAO);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);



            if (clicked_mass_indices.size() != 0)
            {

                glm::vec3 arrow_pos(0.0);
                GLuint simple_sphere_indices[6] = { 0, 1, 2, 1, 2, 3 };
                GLuint simple_sphere_VAO, simple_sphere_EBO;
                glGenVertexArrays(1, &simple_sphere_VAO);
                glGenBuffers(1, &simple_sphere_EBO);
                glBindVertexArray(simple_sphere_VAO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, simple_sphere_EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(simple_sphere_indices), &simple_sphere_indices[0], GL_STATIC_DRAW);
                glBindVertexArray(0);

                glUseProgram(mass_shader.ID);
                glm::mat4 mass_model(1.0f);
                glUniformMatrix4fv(glGetUniformLocation(mass_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(mass_model));
                glUniformMatrix4fv(glGetUniformLocation(mass_shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(glGetUniformLocation(mass_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
                glUniform1f(glGetUniformLocation(mass_shader.ID, "radius"), 0.1);
                for (int i = 0; i < clicked_mass_indices.size(); i++)
                    arrow_pos += implicit_pos[clicked_mass_indices[i]];
                arrow_pos = glm::vec3(arrow_pos.x / clicked_mass_indices.size(), arrow_pos.y / clicked_mass_indices.size(), arrow_pos.z / clicked_mass_indices.size());
                glUniform3f(glGetUniformLocation(mass_shader.ID, "center"), arrow_pos.x, arrow_pos.y, arrow_pos.z);
                glBindVertexArray(simple_sphere_VAO);
                glDrawElements(GL_TRIANGLES, sizeof(simple_sphere_indices), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);

                //make arrow not stay with force point
                arrow_pos += glm::vec3(force_info.x * 0.4 , force_info.y * 0.4, force_info.z * 0.4);
                //rotate force arrow towards force direction
                glm::mat4 R = BuildRotationMatrix(glm::vec3(0.0f, 1.0f, 0.0f), force_info);
                glUseProgram(sphereshader.ID);
                glm::mat4 arrow_cone_model(1.0f);
                arrow_cone_model = glm::translate(arrow_cone_model, arrow_pos);
                arrow_cone_model = arrow_cone_model * R;
                arrow_cone_model = glm::scale(arrow_cone_model, glm::vec3(0.15f, 0.15f, 0.15f));
                arrow_cone_model = glm::translate(arrow_cone_model, glm::vec3(0.0f, 2.0f, 0.0f));
                glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(arrow_cone_model));
                glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
                glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), point_color.x, point_color.y, point_color.z);
                glUniform3f(glGetUniformLocation(sphereshader.ID, "lightPos"), point_pos.x, point_pos.y, point_pos.z);
                glUniform3f(glGetUniformLocation(sphereshader.ID, "objectColor"), 66.0 / 255.0f, 165.0 / 255.0f, 159.0 / 255.0f);
                glUniform3f(glGetUniformLocation(sphereshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
                glBindVertexArray(cone_VAO);
                glDrawElements(GL_TRIANGLES, indices1.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);

                glm::mat4 arrow_cylinder_model(1.0f);
                arrow_cylinder_model = glm::translate(arrow_cylinder_model, arrow_pos);
                arrow_cylinder_model = arrow_cylinder_model * R;
                arrow_cylinder_model = glm::scale(arrow_cylinder_model, glm::vec3(0.15f, 0.15f, 0.15f));
                glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(arrow_cylinder_model));
                glBindVertexArray(cylinder_VAO1);
                glDrawElements(GL_TRIANGLES, indices3.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);

                glDisable(GL_BLEND);
            }

            //render point light
            if (point_light == true && hide_point_light == false)
            {
                glUseProgram(sphereshader.ID);
                glm::mat4 pl_model(1.0f);
                point_pos = point_pos + delta_x + delta_y;
                pl_model = glm::translate(pl_model, point_pos);
                pl_model = glm::scale(pl_model, glm::vec3(0.1f, 0.1f, 0.1f));
                glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(pl_model));
                glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
                glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), 1.0f, 1.0f, 1.0f);
                glUniform3f(glGetUniformLocation(sphereshader.ID, "objectColor"), point_color.x, point_color.y, point_color.z);
                glUniform3f(glGetUniformLocation(sphereshader.ID, "lightPos"), 0.0f, 0.0f, 10.0f);
                glUniform3f(glGetUniformLocation(sphereshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
                glBindVertexArray(sphere_VAO);
                glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);

                //bounding box of point light
                pl_info = sphere_VAO;
                glm::vec4 temp_coord = glm::vec4(projection * view * pl_model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
                glm::vec3 screen_coord = glm::vec3(temp_coord.x / temp_coord.w, temp_coord.y / temp_coord.w, temp_coord.z / temp_coord.w);
                boxes[pl_info].clear();
                boxes[pl_info].push_back(screen_coord.x);
                boxes[pl_info].push_back(screen_coord.y);
                boxes[pl_info].push_back(screen_coord.z);
                glm::vec4 bbox_coord = glm::vec4(projection * view * pl_model * glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
                float width = abs(bbox_coord.x / bbox_coord.w - screen_coord.x);
                float height = abs(bbox_coord.y / bbox_coord.w - screen_coord.y);
                boxes[pl_info].push_back(width);
                boxes[pl_info].push_back(height);
            }

            //render direcitonal light 
            if (directional_light == true && hide_directional_light == false)
            {
                glm::vec3 temp_direction = glm::normalize(directional_light_direction);
                float angle_t = atan(temp_direction.z / temp_direction.x);
                float angle_p = acos(temp_direction.y);
                glUseProgram(sphereshader.ID);
                glm::mat4 cone_model_1(1.0f);
                cone_model_1 = glm::translate(cone_model_1, glm::vec3(3.0f, 1.0f, 1.0f));
                cone_model_1 = glm::rotate(cone_model_1, -angle_t, glm::vec3(0.0f, 1.0f, 0.0f));
                cone_model_1 = glm::rotate(cone_model_1, angle_p, glm::vec3(0.0f, 0.0f, 1.0f));
                cone_model_1 = glm::scale(cone_model_1, glm::vec3(0.1f, 0.1f, 0.1f));
                glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(cone_model_1));
                glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
                glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), 1.0f, 1.0f, 1.0f);
                glUniform3f(glGetUniformLocation(sphereshader.ID, "lightPos"), 0.0f, 0.0f, 10.0f);
                glUniform3f(glGetUniformLocation(sphereshader.ID, "objectColor"), 66.0 / 255.0f, 165.0 / 255.0f, 159.0 / 255.0f);
                glUniform3f(glGetUniformLocation(sphereshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
                glBindVertexArray(cone_VAO);
                glDrawElements(GL_TRIANGLES, indices1.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);


                glm::mat4 cone_model_2(1.0f);
                cone_model_2 = glm::translate(cone_model_2, glm::vec3(3.0f, 0.0f, 1.0f));
                cone_model_2 = glm::rotate(cone_model_2, -angle_t, glm::vec3(0.0f, 1.0f, 0.0f));
                cone_model_2 = glm::rotate(cone_model_2, angle_p, glm::vec3(0.0f, 0.0f, 1.0f));
                cone_model_2 = glm::scale(cone_model_2, glm::vec3(0.1f, 0.1f, 0.1f));
                glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(cone_model_2));
                glBindVertexArray(cone_VAO);
                glDrawElements(GL_TRIANGLES, indices1.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);


                glm::mat4 cone_model_3(1.0f);
                cone_model_3 = glm::translate(cone_model_3, glm::vec3(3.0f, -1.0f, 1.0f));
                cone_model_3 = glm::rotate(cone_model_3, -angle_t, glm::vec3(0.0f, 1.0f, 0.0f));
                cone_model_3 = glm::rotate(cone_model_3, angle_p, glm::vec3(0.0f, 0.0f, 1.0f));
                cone_model_3 = glm::scale(cone_model_3, glm::vec3(0.1f, 0.1f, 0.1f));
                glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(cone_model_3));
                glBindVertexArray(cone_VAO);
                glDrawElements(GL_TRIANGLES, indices1.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);

                glm::mat4 cylinder_model_1(1.0f);
                cylinder_model_1 = glm::translate(cylinder_model_1, glm::vec3(3.0f, 1.0f, 1.0f));
                cylinder_model_1 = glm::rotate(cylinder_model_1, -angle_t, glm::vec3(0.0f, 1.0f, 0.0f));
                cylinder_model_1 = glm::rotate(cylinder_model_1, angle_p, glm::vec3(0.0f, 0.0f, 1.0f));
                cylinder_model_1 = glm::scale(cylinder_model_1, glm::vec3(0.1f, 0.1f, 0.1f));
                cylinder_model_1 = glm::translate(cylinder_model_1, glm::vec3(0.0f, -2.0f, 0.0f));
                glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(cylinder_model_1));
                glBindVertexArray(cylinder_VAO1);
                glDrawElements(GL_TRIANGLES, indices3.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);


                glm::mat4 cylinder_model_2(1.0f);
                cylinder_model_2 = glm::translate(cylinder_model_2, glm::vec3(3.0f, 0.0f, 1.0f));
                cylinder_model_2 = glm::rotate(cylinder_model_2, -angle_t, glm::vec3(0.0f, 1.0f, 0.0f));
                cylinder_model_2 = glm::rotate(cylinder_model_2, angle_p, glm::vec3(0.0f, 0.0f, 1.0f));
                cylinder_model_2 = glm::scale(cylinder_model_2, glm::vec3(0.1f, 0.1f, 0.1f));
                cylinder_model_2 = glm::translate(cylinder_model_2, glm::vec3(0.0f, -2.0f, 0.0f));
                glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(cylinder_model_2));
                glBindVertexArray(cylinder_VAO1);
                glDrawElements(GL_TRIANGLES, indices3.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);


                glm::mat4 cylinder_model_3(1.0f);
                cylinder_model_3 = glm::translate(cylinder_model_3, glm::vec3(3.0f, -1.0f, 1.0f));
                cylinder_model_3 = glm::rotate(cylinder_model_3, -angle_t, glm::vec3(0.0f, 1.0f, 0.0f));
                cylinder_model_3 = glm::rotate(cylinder_model_3, angle_p, glm::vec3(0.0f, 0.0f, 1.0f));
                cylinder_model_3 = glm::scale(cylinder_model_3, glm::vec3(0.1f, 0.1f, 0.1f));
                cylinder_model_3 = glm::translate(cylinder_model_3, glm::vec3(0.0f, -2.0f, 0.0f));
                glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(cylinder_model_3));
                glBindVertexArray(cylinder_VAO1);
                glDrawElements(GL_TRIANGLES, indices3.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);


                glUseProgram(circleshader.ID);
                glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(dir_circle_model));
                glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
                if (dir_xy_min_distance < 0.0005 && dir_xy_min_distance < dir_xz_min_distance && dir_xy_min_distance < dir_yz_min_distance)
                    glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 0.0f, 0.0f, 1.0f);
                else
                    glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 0.0f, 0.0f, 0.3f);
                glBindVertexArray(dir_circle_VAO);
                glDrawElements(GL_LINE_LOOP, dir_circle_indices.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);



                glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(dir_circle_model1));
                glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
                if (dir_xz_min_distance < 0.0005 && dir_xz_min_distance < dir_xy_min_distance && dir_xz_min_distance < dir_yz_min_distance)
                    glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 1.0f, 0.0f, 0.0f);
                else
                    glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 0.3f, 0.0f, 0.0f);
                glBindVertexArray(dir_circle_VAO1);
                glDrawElements(GL_LINE_LOOP, dir_circle_indices1.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);



                glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(dir_circle_model2));
                glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(glGetUniformLocation(circleshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
                if (dir_yz_min_distance < 0.0005 && dir_yz_min_distance < dir_xz_min_distance && dir_yz_min_distance < dir_xy_min_distance)
                    glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 0.0f, 1.0f, 0.0f);
                else
                    glUniform3f(glGetUniformLocation(circleshader.ID, "lightColor"), 0.0f, 0.3f, 0.0f);
                glBindVertexArray(dir_circle_VAO2);
                glDrawElements(GL_LINE_LOOP, dir_circle_indices2.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
            }
        }

        if (show_dinosaur == true)
        {
            auto t1 = high_resolution_clock::now();
            //dinosaur_ani->obj_animation(0.02, glm::vec3(0.0, -0.1f, 0.0), dinosaur_spring_pos, dinosaur_vertices, dinosaur_normals, dinosaur_connection);
            Newton_ani->obj_Newton_animation(0.02, glm::vec3(0.0, -0.1f, 0.0), dinosaur_spring_pos, dinosaur_connection);

            GLuint spring_VAO, spring_VBO;
            glGenVertexArrays(1, &spring_VAO);
            glGenBuffers(1, &spring_VBO);
            glBindVertexArray(spring_VAO);
            glBindBuffer(GL_ARRAY_BUFFER, spring_VBO);
            glBufferData(GL_ARRAY_BUFFER, dinosaur_spring_pos.size() * sizeof(glm::vec3), &dinosaur_spring_pos[0], GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
            glBindVertexArray(0);

            glUseProgram(spring_shader.ID);
            glm::mat4 spring_model(1.0f);
            glUniformMatrix4fv(glGetUniformLocation(spring_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(spring_model));
            glUniformMatrix4fv(glGetUniformLocation(spring_shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(spring_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform3f(glGetUniformLocation(spring_shader.ID, "object_color"), 1.0f, 0.0f, 0.0f);
            glBindVertexArray(spring_VAO);
            glDrawArrays(GL_LINES, 0, dinosaur_spring_pos.size());
            glBindVertexArray(0);
            glDeleteVertexArrays(1, &spring_VAO);
            glDeleteBuffers(1, &spring_VBO);

            //GLuint bar_EBO, bar_VBO, bar_VAO, bar_normal_VBO;
            //glGenVertexArrays(1, &bar_VAO);
            //glGenBuffers(1, &bar_VBO);
            //glGenBuffers(1, &bar_EBO);
            //glGenBuffers(1, &bar_normal_VBO);
            //glBindVertexArray(bar_VAO);
            //glBindBuffer(GL_ARRAY_BUFFER, bar_VBO);
            //glBufferData(GL_ARRAY_BUFFER, dinosaur_vertices.size() * sizeof(glm::vec3), &dinosaur_vertices[0], GL_STATIC_DRAW);
            //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
            //glEnableVertexAttribArray(0);
            //glBindBuffer(GL_ARRAY_BUFFER, bar_normal_VBO);
            //glBufferData(GL_ARRAY_BUFFER, dinosaur_normals.size() * sizeof(glm::vec3), &dinosaur_normals[0], GL_STATIC_DRAW);
            //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
            //glEnableVertexAttribArray(1);
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bar_EBO);
            //glBufferData(GL_ELEMENT_ARRAY_BUFFER, dinosaur_indices.size() * sizeof(GLuint), &dinosaur_indices[0], GL_STATIC_DRAW);
            //glBindVertexArray(0);

            //glUseProgram(sphereshader.ID);
            //glm::mat4 bar_model(1.0f);
            //glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(bar_model));
            //glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            //glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            //glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), 1.0f, 1.0f, 1.0f);
            //glUniform3f(glGetUniformLocation(sphereshader.ID, "lightPos"), 0.0f, 0.0f, 5.0f);
            //glUniform3f(glGetUniformLocation(sphereshader.ID, "objectColor"), 66.0 / 255.0f, 165.0 / 255.0f, 159.0 / 255.0f);
            //glUniform3f(glGetUniformLocation(sphereshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            //glBindVertexArray(bar_VAO);
            //glDrawElements(GL_TRIANGLES, dinosaur_indices.size(), GL_UNSIGNED_INT, 0);
            //glBindVertexArray(0);
            // 
            //glDeleteVertexArrays(1, &bar_VAO);
            //glDeleteBuffers(1, &bar_VBO);
            //glDeleteBuffers(1, &bar_EBO);
            //glDeleteBuffers(1, &bar_normal_VBO);

            auto t2 = high_resolution_clock::now();
            duration <double, std::milli> time = t2 - t1;
            std::cout << time.count() << "ms\n";
        }

        if (show_bar == true)
        {
            //auto t1 = high_resolution_clock::now();
            //bar_ani->obj_animation(0.02, glm::vec3(0.0, -0.1f, 0.0), bar_spring_pos, bar_vertices, bar_normals, bar_connection);

            //GLuint spring_VAO, spring_VBO;
            //glGenVertexArrays(1, &spring_VAO);
            //glGenBuffers(1, &spring_VBO);
            //glBindVertexArray(spring_VAO);
            //glBindBuffer(GL_ARRAY_BUFFER, spring_VBO);
            //glBufferData(GL_ARRAY_BUFFER, bar_spring_pos.size() * sizeof(glm::vec3), &bar_spring_pos[0], GL_STATIC_DRAW);
            //glEnableVertexAttribArray(0);
            //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
            //glBindVertexArray(0);

            //glUseProgram(spring_shader.ID);
            //glm::mat4 spring_model(1.0f);
            //glUniformMatrix4fv(glGetUniformLocation(spring_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(spring_model));
            //glUniformMatrix4fv(glGetUniformLocation(spring_shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            //glUniformMatrix4fv(glGetUniformLocation(spring_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            //glUniform3f(glGetUniformLocation(spring_shader.ID, "object_color"), 1.0f, 0.0f, 0.0f);
            //glBindVertexArray(spring_VAO);
            //glDrawArrays(GL_LINES, 0, bar_spring_pos.size());
            //glBindVertexArray(0);

            //auto t2 = high_resolution_clock::now();
            //duration <double, std::milli> time = t2 - t1;
            //std::cout << time.count() << "ms\n";


            //GLuint bar_EBO, bar_VBO, bar_VAO, bar_normal_VBO;
            //glGenVertexArrays(1, &bar_VAO);
            //glGenBuffers(1, &bar_VBO);
            //glGenBuffers(1, &bar_EBO);
            //glGenBuffers(1, &bar_normal_VBO);
            //glBindVertexArray(bar_VAO);
            //glBindBuffer(GL_ARRAY_BUFFER, bar_VBO);
            //glBufferData(GL_ARRAY_BUFFER, bar_vertices.size() * sizeof(glm::vec3), &bar_vertices[0], GL_STATIC_DRAW);
            //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
            //glEnableVertexAttribArray(0);
            //glBindBuffer(GL_ARRAY_BUFFER, bar_normal_VBO);
            //glBufferData(GL_ARRAY_BUFFER, bar_normals.size() * sizeof(glm::vec3), &bar_normals[0], GL_STATIC_DRAW);
            //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
            //glEnableVertexAttribArray(1);
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bar_EBO);
            //glBufferData(GL_ELEMENT_ARRAY_BUFFER, bar_indices.size() * sizeof(GLuint), &bar_indices[0], GL_STATIC_DRAW);
            //glBindVertexArray(0);

            //glUseProgram(sphereshader.ID);
            //glm::mat4 bar_model(1.0f);
            //glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(bar_model));
            //glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            //glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            //glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), 1.0f, 1.0f, 1.0f);
            //glUniform3f(glGetUniformLocation(sphereshader.ID, "lightPos"), 0.0f, 0.0f, 5.0f);
            //glUniform3f(glGetUniformLocation(sphereshader.ID, "objectColor"), 66.0 / 255.0f, 165.0 / 255.0f, 159.0 / 255.0f);
            //glUniform3f(glGetUniformLocation(sphereshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            //glBindVertexArray(bar_VAO);
            //glDrawElements(GL_TRIANGLES, bar_indices.size(), GL_UNSIGNED_INT, 0);
            //glBindVertexArray(0);


        }
        //xyz axis in the left bottom of screen
        glViewport(0, 0, WIDTH / 4, HEIGHT / 4);
        {
            glUseProgram(sphereshader.ID);
            //x轴
            glm::mat4 xaxis_rotation = BuildRotationMatrix(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            glm::mat4 xaxis_cone_model(1.0f);
            xaxis_cone_model = glm::translate(xaxis_cone_model, glm::vec3(0.0f, -0.5f, 0.0f));
            xaxis_cone_model = xaxis_cone_model * xaxis_rotation;
            xaxis_cone_model = glm::scale(xaxis_cone_model, glm::vec3(0.3f, 0.3f, 0.3f));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(xaxis_cone_model));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), 1.0f, 1.0f, 1.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "objectColor"), 173.0 / 255.0f, 101.0f / 255.0f, 85.0f / 255.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightPos"), 4.0f, 2.0f, 4.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            glBindVertexArray(cone_VAO);
            glDrawElements(GL_TRIANGLES, indices1.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            //y轴
            glm::mat4 yaxis_cone_model(1.0f);
            yaxis_cone_model = glm::translate(yaxis_cone_model, glm::vec3(-1.0f, 0.5f, 0.0f));
            yaxis_cone_model = glm::scale(yaxis_cone_model, glm::vec3(0.3f, 0.3f, 0.3f));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(yaxis_cone_model));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), 1.0f, 1.0f, 1.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "objectColor"), 143.0 / 255.0f, 164.0 / 255.0f, 63.0 / 255.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightPos"), 4.0f, 2.0f, 4.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            glBindVertexArray(cone_VAO);
            glDrawElements(GL_TRIANGLES, indices1.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            //z轴
            glm::mat4 zaxis_rotation = BuildRotationMatrix(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            glm::mat4 zaxis_cone_model(1.0f);
            zaxis_cone_model = glm::translate(zaxis_cone_model, glm::vec3(-1.0f, -0.5f, 1.0f));
            zaxis_cone_model = zaxis_cone_model * zaxis_rotation;
            zaxis_cone_model = glm::scale(zaxis_cone_model, glm::vec3(0.3f, 0.3f, 0.3f));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(zaxis_cone_model));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), 1.0f, 1.0f, 1.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "objectColor"), 66.0 / 255.0f, 165.0 / 255.0f, 159.0 / 255.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightPos"), 4.0f, 2.0f, 4.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            glBindVertexArray(cone_VAO);
            glDrawElements(GL_TRIANGLES, indices1.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);


            glm::mat4 xaxis_cylinder_model(1.0f);
            xaxis_cylinder_model = glm::translate(xaxis_cylinder_model, glm::vec3(0.0f, -0.5f, 0.0f));
            xaxis_cylinder_model = xaxis_cylinder_model * xaxis_rotation;
            xaxis_cylinder_model = glm::scale(xaxis_cylinder_model, glm::vec3(0.3f, 0.3f, 0.3f));
            xaxis_cylinder_model = glm::translate(xaxis_cylinder_model, glm::vec3(0.0f, -1.5f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(xaxis_cylinder_model));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), 1.0f, 1.0f, 1.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "objectColor"), 173.0 / 255.0f, 101.0f / 255.0f, 85.0f / 255.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightPos"), 4.0f, 2.0f, 4.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            glBindVertexArray(cylinder_VAO);
            glDrawElements(GL_TRIANGLES, indices2.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);


            glm::mat4 yaxis_cylinder_model(1.0f);
            yaxis_cylinder_model = glm::translate(yaxis_cylinder_model, glm::vec3(-1.0f, 0.5f, 0.0f));
            yaxis_cylinder_model = glm::scale(yaxis_cylinder_model, glm::vec3(0.3f, 0.3f, 0.3f));
            yaxis_cylinder_model = glm::translate(yaxis_cylinder_model, glm::vec3(0.0f, -1.5f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(yaxis_cylinder_model));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), 1.0f, 1.0f, 1.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "objectColor"), 143.0 / 255.0f, 164.0 / 255.0f, 63.0 / 255.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightPos"), 4.0f, 2.0f, 4.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            glBindVertexArray(cylinder_VAO);
            glDrawElements(GL_TRIANGLES, indices2.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);


            glm::mat4 zaxis_cylinder_model(1.0f);
            zaxis_cylinder_model = glm::translate(zaxis_cylinder_model, glm::vec3(-1.0f, -0.5f, 1.0f));
            zaxis_cylinder_model = zaxis_cylinder_model * zaxis_rotation;
            zaxis_cylinder_model = glm::scale(zaxis_cylinder_model, glm::vec3(0.3f, 0.3f, 0.3f));
            zaxis_cylinder_model = glm::translate(zaxis_cylinder_model, glm::vec3(0.0f, -1.5f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(zaxis_cylinder_model));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), 1.0f, 1.0f, 1.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "objectColor"), 66.0 / 255.0f, 165.0 / 255.0f, 159.0 / 255.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightPos"), 4.0f, 2.0f, 4.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            glBindVertexArray(cylinder_VAO);
            glDrawElements(GL_TRIANGLES, indices2.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            //origin sphere
            glUseProgram(sphereshader.ID);
            glm::mat4 axis_sphere_model = glm::mat4(1.0f);
            axis_sphere_model = glm::translate(axis_sphere_model, glm::vec3(-1.0f, -0.5f, 0.0f));
            axis_sphere_model = glm::scale(axis_sphere_model, glm::vec3(0.3f, 0.3f, 0.3f));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(axis_sphere_model));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(sphereshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightColor"), 0.3f, 0.4f, 0.5f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "objectColor"), 1.0f, 1.0f, 1.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "lightPos"), 4.0f, 2.0f, 4.0f);
            glUniform3f(glGetUniformLocation(sphereshader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            glBindVertexArray(sphere_VAO);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
        glViewport(0, 0, WIDTH, HEIGHT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }



    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

int render_objects(std::string filename)
{
    object_vertices.clear();
    object_normals.clear();
    object_faces.clear();
    vertice_size = 0;
    face_size = 0;

    glm::vec3 Vertex;
    glm::vec3 Normal;
    GLfloat Face[3];
    std::vector<GLfloat> Vertices;
    std::vector<glm::vec3> Normals;
    std::vector<GLuint> Faces;
    std::vector<float> Tri_areas;
    std::vector <GLfloat> v_Normals;
    std::string Line;
    std::ifstream File;
    std::map<int, std::vector<int>> refer;
    int line = 0;

    File.open(filename);
    float x_min = 0.0f, x_max = 0.0f, y_min = 0.0f, y_max = 0.0f, z_min = 0.0f, z_max = 0.0f;
    float x_range = 0.0f, y_range = 0.0f, z_range = 0.0f;
    while (std::getline(File, Line)) 
    {
        if (Line == "" || Line[0] == '#')
            continue;

        if (Line.c_str()[0] == 'v') 
        {
            vertice_size++;
            sscanf_s(Line.c_str(), "%*s %f %f %f", &Vertex.x, &Vertex.y, &Vertex.z);
            if (Vertex.x > x_max)
                x_max = Vertex.x;
            else if (Vertex.x < x_min)
                x_min = Vertex.x;
            if (Vertex.y > y_max)
                y_max = Vertex.y;
            else if (Vertex.y < y_min)
                y_min = Vertex.y;
            if (Vertex.z > z_max)
                z_max = Vertex.z;
            else if (Vertex.z < z_min)
                z_min = Vertex.z;

            Vertices.push_back(Vertex.x);
            Vertices.push_back(Vertex.y);
            Vertices.push_back(Vertex.z);
            // vertex.x, vertex.y, vertex.z
            object_vertices.push_back(Vertex.x);
            object_vertices.push_back(Vertex.y);
            object_vertices.push_back(Vertex.z);
        }

        else if (Line.c_str()[0] == 'f') 
        {
            face_size++;
            sscanf_s(Line.c_str(), "%*s %f %f %f", &Face[0], &Face[1], &Face[2]);
            Faces.push_back(Face[0] - 1); //在文件中顶点顺序从1开始
            Faces.push_back(Face[1] - 1);
            Faces.push_back(Face[2] - 1);
            object_faces.push_back(Face[0] - 1); 
            object_faces.push_back(Face[1] - 1);
            object_faces.push_back(Face[2] - 1);
            //face.vertex1, face.vertex2, face.vertex3
            refer[(Face[0] - 1)].push_back(line); //每个顶点所相邻的三角形序号
            refer[(Face[1] - 1)].push_back(line);
            refer[(Face[2] - 1)].push_back(line);
            line++;
        };
    };
    
  
    if (x_min < -1 || x_max > 1 || y_min < -1 || y_max > 1 || z_min < -1 || z_max > 1)
    {
        x_range = x_max - x_min;
        y_range = y_max - y_min;
        z_range = z_max - z_min;
        for (int i = 0; i < Vertices.size(); i++)
        {
            if (i % 3 == 0)
            {
                Vertices[i] = Vertices[i] / x_range;
                object_vertices[i] = object_vertices[i] / x_range;
            }

            if (i % 3 == 1)
            {
                Vertices[i] = Vertices[i] / y_range;
                object_vertices[i] = object_vertices[i] / y_range;
            }

            if (i % 3 == 2)
            {
                Vertices[i] = Vertices[i] / z_range;
                object_vertices[i] = object_vertices[i] / z_range;
            }
        }
    }




    float x_medium = (x_max + x_min) / 2.0; // 物体x轴中心
    float y_medium = (y_max + y_min) / 2.0; //物体y轴中心
    float z_medium = (z_max + z_min) / 2.0; //物体z轴中心

    //计算每个面的法向量和面积
    for (int i = 0; i < face_size; i++) {
        glm::vec3 p1(Vertices[3 * Faces[3 * i]], Vertices[3 * Faces[3 * i] + 1], Vertices[3 * Faces[3 * i] + 2]);
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
        object_normals.push_back(vertex_normal.x);//normal vectors of vertices
        object_normals.push_back(vertex_normal.y);
        object_normals.push_back(vertex_normal.z);
    }


    GLuint vertex_EBO, location_VBO, vertex_VAO, color_VBO;
    glGenVertexArrays(1, &vertex_VAO);
    glGenBuffers(1, &location_VBO);
    glGenBuffers(1, &vertex_EBO);
    glGenBuffers(1, &color_VBO);

    glBindVertexArray(vertex_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, location_VBO);
    glBufferData(GL_ARRAY_BUFFER, object_vertices.size() * sizeof(GLfloat), &object_vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, color_VBO);
    glBufferData(GL_ARRAY_BUFFER, object_normals.size() * sizeof(GLfloat), &object_normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, object_faces.size() * sizeof(GLuint), &object_faces[0], GL_STATIC_DRAW);
    
    glBindVertexArray(0);

    return vertex_VAO;
}

void showMainMenu()
{
    bool open = false, save = false;
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Menu"))
        {
            if (ImGui::MenuItem("Open", NULL))
                open = true;
            if (ImGui::MenuItem("Save", NULL))
                save = true;

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    //Remember the name to ImGui::OpenPopup() and showFileDialog() must be same...
    if (open)
        ImGui::OpenPopup("Open File");
    if (save)
        ImGui::OpenPopup("Save File");

    /* Optional third parameter. Support opening only compressed rar/zip files.
     * Opening any other file will show error, return false and won't close the dialog.
     */
    if (file_dialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ".obj"))
    {
        std::cout << file_dialog.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
        std::cout << file_dialog.selected_path << std::endl;    // The absolute path to the selected file
    }
    if (file_dialog.showFileDialog("Save File", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, ImVec2(700, 310), ".png,.jpg,.bmp"))
    {
        std::cout << file_dialog.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
        std::cout << file_dialog.selected_path << std::endl;    // The absolute path to the selected file
        std::cout << file_dialog.ext << std::endl;              // Access ext separately (For SAVE mode)
        //Do writing of files based on extension here
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// 画圆柱底部的圆
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
int draw_cone(float radius, float height, int sectorcount)
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
        if (h == -height / 2.0)
            real_radius = radius;
        if (h == height / 2.0)
            real_radius = 0;
        vertices1.push_back(0);
        vertices1.push_back(h);
        vertices1.push_back(0);

        normals1.push_back(0);
        normals1.push_back(ny);
        normals1.push_back(0);

        for (int j = 0, k = 0; j < sectorcount; j++, k += 3)
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

    GLuint cone_VAO, cone_VBO, cone_color_VBO, cone_EBO;
    glGenVertexArrays(1, &cone_VAO);
    glGenBuffers(1, &cone_VBO);
    glGenBuffers(1, &cone_EBO);
    glGenBuffers(1, &cone_color_VBO);

    glBindVertexArray(cone_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, cone_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices1.size() * sizeof(float), &vertices1[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, cone_color_VBO);
    glBufferData(GL_ARRAY_BUFFER, normals1.size() * sizeof(float), &normals1[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(1);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cone_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices1.size() * sizeof(int), &indices1[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    return cone_VAO;
}

//画圆柱
int draw_cylinder(float radius, float height, int sectorcount)
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

    GLuint cylinder_VAO, cylinder_VBO, cylinder_color_VBO, cylinder_EBO;
    glGenVertexArrays(1, &cylinder_VAO);
    glGenBuffers(1, &cylinder_VBO);
    glGenBuffers(1, &cylinder_color_VBO);
    glGenBuffers(1, &cylinder_EBO);

    glBindVertexArray(cylinder_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, cylinder_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(float), &vertices2[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, cylinder_color_VBO);
    glBufferData(GL_ARRAY_BUFFER, normals2.size() * sizeof(float), &normals2[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinder_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices2.size() * sizeof(int), &indices2[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    return cylinder_VAO;
}

int draw_cylinder1(float radius, float height, int sectorcount)
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
            vertices3.push_back(ux * real_radius);
            vertices3.push_back(h);
            vertices3.push_back(uz * real_radius);

            normals3.push_back(ux);
            normals3.push_back(uz);
            normals3.push_back(uy);
        }
    }
    int basecenterindex = (int)vertices2.size() / 3;
    int topcenterindex = basecenterindex + sectorcount + 1;
    for (int i = 0; i < 2; i++)
    {
        float h = -height / 2.0 + i * height;
        float ny = -1 + i * 2;
        vertices3.push_back(0);
        vertices3.push_back(h);
        vertices3.push_back(0);

        normals3.push_back(0);
        normals3.push_back(ny);
        normals3.push_back(0);

        for (int j = 0, k = 0; j < sectorcount; j++, k += 3)
        {
            float ux = unitvertices[k];
            float uz = unitvertices[k + 2];
            vertices3.push_back(ux * real_radius);
            vertices3.push_back(h);
            vertices3.push_back(uz * real_radius);

            normals3.push_back(0);
            normals3.push_back(ny);
            normals3.push_back(0);
        }
    }

    int k1 = 0;
    int k2 = sectorcount + 1;
    for (int i = 0; i < sectorcount; i++, k1++, k2++)
    {
        indices3.push_back(k1);
        indices3.push_back(k1 + 1);
        indices3.push_back(k2);
        indices3.push_back(k2);
        indices3.push_back(k1 + 1);
        indices3.push_back(k2 + 1);
    }

    for (int i = 0, k = basecenterindex + 1; i < sectorcount; i++, k++)
    {
        if (i < sectorcount - 1)
        {
            indices3.push_back(basecenterindex);
            indices3.push_back(k + 1);
            indices3.push_back(k);
        }
        else
        {
            indices3.push_back(basecenterindex);
            indices3.push_back(basecenterindex + 1);
            indices3.push_back(k);
        }
    }

    for (int i = 0, k = topcenterindex + 1; i < sectorcount; i++, k++)
    {
        if (i < sectorcount - 1)
        {
            indices3.push_back(topcenterindex);
            indices3.push_back(k);
            indices3.push_back(k + 1);
        }
        else
        {
            indices3.push_back(topcenterindex);
            indices3.push_back(k);
            indices3.push_back(topcenterindex + 1);
        }
    }

    GLuint cylinder_VAO, cylinder_VBO, cylinder_color_VBO, cylinder_EBO;
    glGenVertexArrays(1, &cylinder_VAO);
    glGenBuffers(1, &cylinder_VBO);
    glGenBuffers(1, &cylinder_color_VBO);
    glGenBuffers(1, &cylinder_EBO);

    glBindVertexArray(cylinder_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, cylinder_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices3.size() * sizeof(float), &vertices3[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, cylinder_color_VBO);
    glBufferData(GL_ARRAY_BUFFER, normals3.size() * sizeof(float), &normals3[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinder_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices3.size() * sizeof(int), &indices3[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    return cylinder_VAO;
}
//画球体
int draw_sphere(float radius, int sectorCount, int stackCount)
{
    double PI = 3.14159;
    float x, y, z, xz;
    float nx, ny, nz;
    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float lengthInv = 1.0f / radius;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; i++)
    {
        //由上到下
        stackAngle = PI / 2 - i * stackStep;
        xz = radius * cos(stackAngle);
        y = radius * sin(stackAngle);

        for (int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;
            x = xz * cos(sectorAngle);
            z = xz * sin(sectorAngle);
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

//画旋转圆
int draw_xy_circle(GLfloat x, GLfloat y, GLfloat radius, int num_segments) //xy平面
{
    float cx = x;
    float cy = y;
    float rx, ry;
    GLfloat PI = acos(-1);

    for (int i = 0; i <= num_segments; i++)
    {
        rx = cx + radius * cos(i * 2 * PI / num_segments);
        ry = cy + radius * sin(i * 2 * PI / num_segments);
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

int draw_xz_circle(GLfloat x, GLfloat z, GLfloat radius, int num_segments) //xz平面
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

int draw_yz_circle(GLfloat y, GLfloat z, GLfloat radius, int num_segments) //yz平面
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

int draw_xy_circle1(GLfloat x, GLfloat y, GLfloat radius, int num_segments) //xy平面
{
    float cx = x;
    float cy = y;
    float rx, ry;
    GLfloat PI = acos(-1);

    for (int i = 0; i <= num_segments; i++)
    {
        rx = cx + radius * cos(i * 2 * PI / num_segments);
        ry = cy + radius * sin(i * 2 * PI / num_segments);
        dir_circle_vertices.push_back(rx);
        dir_circle_vertices.push_back(ry);
        dir_circle_vertices.push_back(0);
        dir_circle_indices.push_back(i);
    }

    GLuint circle_VAO, circle_VBO, circle_EBO;
    glGenVertexArrays(1, &circle_VAO);
    glGenBuffers(1, &circle_VBO);
    glGenBuffers(1, &circle_EBO);

    glBindVertexArray(circle_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, circle_VBO);
    glBufferData(GL_ARRAY_BUFFER, dir_circle_vertices.size() * sizeof(float), &dir_circle_vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, circle_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, dir_circle_indices.size() * sizeof(int), &dir_circle_indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    return circle_VAO;
}

int draw_xz_circle1(GLfloat x, GLfloat z, GLfloat radius, int num_segments) //xz平面
{
    float cx = x;
    float cz = z;
    float rx, rz;
    GLfloat PI = acos(-1);

    for (int i = 0; i <= num_segments; i++)
    {
        rx = cx + radius * cos(i * 2 * PI / num_segments);
        rz = cz + radius * sin(-i * 2 * PI / num_segments);
        dir_circle_vertices1.push_back(rx);
        dir_circle_vertices1.push_back(0);
        dir_circle_vertices1.push_back(rz);
        dir_circle_indices1.push_back(i);
    }

    GLuint circle_VAO1, circle_VBO1, circle_EBO1;
    glGenVertexArrays(1, &circle_VAO1);
    glGenBuffers(1, &circle_VBO1);
    glGenBuffers(1, &circle_EBO1);

    glBindVertexArray(circle_VAO1);
    glBindBuffer(GL_ARRAY_BUFFER, circle_VBO1);
    glBufferData(GL_ARRAY_BUFFER, dir_circle_vertices1.size() * sizeof(float), &dir_circle_vertices1[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, circle_EBO1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, dir_circle_indices1.size() * sizeof(int), &dir_circle_indices1[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    return circle_VAO1;
}

int draw_yz_circle1(GLfloat y, GLfloat z, GLfloat radius, int num_segments) //yz平面
{
    float cy = y;
    float cz = z;
    float ry, rz;
    GLfloat PI = acos(-1);

    for (int i = 0; i <= num_segments; i++)
    {
        ry = cy + radius * cos(i * 2 * PI / num_segments);
        rz = cz + radius * sin(-i * 2 * PI / num_segments);
        dir_circle_vertices2.push_back(0);
        dir_circle_vertices2.push_back(ry);
        dir_circle_vertices2.push_back(rz);
        dir_circle_indices2.push_back(i);
    }

    GLuint circle_VAO2, circle_VBO2, circle_EBO2;
    glGenVertexArrays(1, &circle_VAO2);
    glGenBuffers(1, &circle_VBO2);
    glGenBuffers(1, &circle_EBO2);

    glBindVertexArray(circle_VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, circle_VBO2);
    glBufferData(GL_ARRAY_BUFFER, dir_circle_vertices2.size() * sizeof(float), &dir_circle_vertices2[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, circle_EBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, dir_circle_indices2.size() * sizeof(int), &dir_circle_indices2[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    return circle_VAO2;
}
bool mouse_flag = false; // 鼠标点击还是释放
bool initial_pickbox_flag = false;
bool end_pickbox_flag = false;
bool x_move_flag = false; // 运动还是停止
bool y_move_flag = false;
bool z_move_flag = false;
bool dir_x_move_flag = false;
bool dir_y_move_flag = false;
bool dir_z_move_flag = false;
double mouseX = 0.0, mouseY = 0.0, r_mouseX = 0.0, r_mouseY = 0.0;
float temp_xpos, temp_ypos;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        mouse_flag = true;
        drag_object = true;
        user_force = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        mouse_flag = false;
        drag_object = false;
        user_force = false;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        initial_pickbox_flag = true;
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        end_pickbox_flag = true;

    if (mouse_flag == true)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        mouseX = xpos / (WIDTH * 0.5f) - 1.0f;
        mouseY = 1.0f - ypos / (HEIGHT * 0.5f);
        temp_xpos = mouseX;
        temp_ypos = mouseY;
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
        for (int i = 0; i < circle_vertices.size() / 3; i++)
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

        for (int i = 0; i < dir_circle_vertices.size() / 3; i++)
        {
            glm::vec4 position3 = projection * view * dir_circle_model * glm::vec4(dir_circle_vertices[3 * i], dir_circle_vertices[3 * i + 1], dir_circle_vertices[3 * i + 2], 1.0f);
            position3 = glm::vec4(position3.x / position3.w, position3.y / position3.w, position3.z / position3.w, 1.0f);
            float distance = pow(mouseX - position3.x, 2) + pow(mouseY - position3.y, 2);
            if (distance < dir_xy_min_distance)
                dir_xy_min_distance = distance; //xy-plane
        }
        for (int j = 0; j < dir_circle_vertices1.size() / 3; j++)
        {
            glm::vec4 position4 = projection * view * dir_circle_model1 * glm::vec4(dir_circle_vertices1[3 * j], dir_circle_vertices1[3 * j + 1], dir_circle_vertices1[3 * j + 2], 1.0f);
            position4 = glm::vec4(position4.x / position4.w, position4.y / position4.w, position4.z / position4.w, 1.0f);
            float distance1 = pow(mouseX - position4.x, 2) + pow(mouseY - position4.y, 2);
            if (distance1 < dir_xz_min_distance)
                dir_xz_min_distance = distance1; //xz-plane
        }
        for (int k = 0; k < dir_circle_vertices2.size() / 3; k++)
        {
            glm::vec4 position5 = projection * view * dir_circle_model2 * glm::vec4(dir_circle_vertices2[3 * k], dir_circle_vertices2[3 * k + 1], dir_circle_vertices2[3 * k + 2], 1.0f);
            position5 = glm::vec4(position5.x / position5.w, position5.y / position5.w, position5.z / position5.w, 1.0f);
            float distance2 = pow(mouseX - position5.x, 2) + pow(mouseY - position5.y, 2);
            if (distance2 < dir_yz_min_distance)
                dir_yz_min_distance = distance2; //yz-plane
        }
    }
    if (mouse_flag == false)
    {
        xz_min_distance = 9999.0;
        xy_min_distance = 9999.0;
        yz_min_distance = 9999.0;
        temp_xpos = 0.0;
        temp_ypos = 0.0;

        dir_xz_min_distance = 9999.0;
        dir_xy_min_distance = 9999.0;
        dir_yz_min_distance = 9999.0;
    }

    if (initial_pickbox_flag == true)
    {
        start_pickbox = true;
        glfwGetCursorPos(window, &initial_pickbox_x, &initial_pickbox_y);
        vipb_x = initial_pickbox_x / (WIDTH * 0.5f) - 1.0f;
        vipb_y = 1.0f - initial_pickbox_y / (HEIGHT * 0.5f);
        initial_pickbox_y = HEIGHT - initial_pickbox_y;
        initial_pickbox_flag = false;
    }

    else if (end_pickbox_flag == true)
    {
        start_pickbox = false;
        glfwGetCursorPos(window, &end_pickbox_x, &end_pickbox_y);
        vepb_x = end_pickbox_x / (WIDTH * 0.5f) - 1.0f;
        vepb_y = 1.0f - end_pickbox_y / (HEIGHT * 0.5f);
       select_points(vipb_x, vipb_y, vepb_x, vepb_y);
        end_pickbox_flag = false;
    }


}

glm::vec3 self_x_axis(1.0f, 0.0f, 0.0f);
glm::vec3 self_y_axis(0.0f, 1.0f, 0.0f);
glm::vec3 self_z_axis(0.0f, 0.0f, 1.0f);

glm::vec3 dir_x_axis(1.0f, 0.0f, 0.0f);
glm::vec3 dir_y_axis(0.0f, 1.0f, 0.0f);
glm::vec3 dir_z_axis(0.0f, 0.0f, 1.0f);

void mouse_movement(GLFWwindow* window)
{
    bool rotate_self_x_axis = false;
    bool rotate_self_y_axis = false;
    bool rotate_self_z_axis = false;
    float total_x_mouse_move = 0;
    float total_y_mouse_move = 0;
    float total_z_mouse_move = 0;

    bool rotate_dir_x_axis = false;
    bool rotate_dir_y_axis = false;
    bool rotate_dir_z_axis = false;
    float dir_total_x_mouse_move = 0;
    float dir_total_y_mouse_move = 0;
    float dir_total_z_mouse_move = 0;

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
            if(x_mouse_move != 0)
                x_move_flag = true; //xz-plane
            if (r_mouseX < -1 || r_mouseX > 1)
            {
                glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);
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
            z_mouse_move = dif_z * senstivity;
            if (z_mouse_move != 0)
                z_move_flag = true; //xy-plane
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
            y_mouse_move = dif_y * senstivity;
            if(y_mouse_move != 0)
                y_move_flag = true; // yz-plane
            if (r_mouseX < -1 || r_mouseX > 1)
            {
                glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);
                temp_xpos = 0.0;
                temp_ypos = 0.0;
            }
        }


        if (x_move_flag == true)
        {
            model = glm::rotate(model, glm::radians(x_mouse_move), self_y_axis);
            circle_model = glm::rotate(circle_model, glm::radians(x_mouse_move), self_y_axis);
            circle_model1 = glm::rotate(circle_model1, glm::radians(x_mouse_move), self_y_axis);
            circle_model2 = glm::rotate(circle_model2, glm::radians(x_mouse_move), self_y_axis);
            total_x_mouse_move += x_mouse_move;
            x_mouse_move = 0.0;
            x_move_flag = false;
            rotate_self_x_axis = true;
        }

        if (y_move_flag == true)
        {
            model = glm::rotate(model, glm::radians(y_mouse_move), self_x_axis);
            circle_model = glm::rotate(circle_model, glm::radians(y_mouse_move), self_x_axis);
            circle_model1 = glm::rotate(circle_model1, glm::radians(y_mouse_move), self_x_axis);
            circle_model2 = glm::rotate(circle_model2, glm::radians(y_mouse_move), self_x_axis);
            total_y_mouse_move += y_mouse_move;
            y_mouse_move = 0.0;
            y_move_flag = false;
            rotate_self_y_axis = true;
        }

        if (z_move_flag == true)
        {
            model = glm::rotate(model, glm::radians(z_mouse_move), self_z_axis);
            circle_model = glm::rotate(circle_model, glm::radians(z_mouse_move), self_z_axis);
            circle_model1 = glm::rotate(circle_model1, glm::radians(z_mouse_move), self_z_axis);
            circle_model2 = glm::rotate(circle_model2, glm::radians(z_mouse_move), self_z_axis);
            total_z_mouse_move += z_mouse_move;
            z_mouse_move = 0.0;
            z_move_flag = false;
            rotate_self_z_axis = true;
        }

        if (dir_xz_min_distance < 0.0005 && dir_xz_min_distance < dir_xy_min_distance && dir_xz_min_distance < dir_yz_min_distance) //xz平面
        {
            double r_xpos, r_ypos;
            glfwGetCursorPos(window, &r_xpos, &r_ypos);
            r_mouseX = r_xpos / (WIDTH * 0.5f) - 1.0f;
            r_mouseY = 1.0f - r_ypos / (HEIGHT * 0.5f);
            float dif_x = r_mouseX - temp_xpos;
            temp_xpos = r_mouseX;
            temp_ypos = r_mouseY;
            float senstivity = 30.0;
            dir_x_mouse_move = dif_x * senstivity;
            if (dir_x_mouse_move != 0)
                dir_x_move_flag = true; //xz-plane
        }

        else if (dir_xy_min_distance < 0.0005 && dir_xy_min_distance < dir_xz_min_distance && dir_xy_min_distance < dir_yz_min_distance) //xy平面
        {
            double r_xpos, r_ypos;
            glfwGetCursorPos(window, &r_xpos, &r_ypos);
            r_mouseX = r_xpos / (WIDTH * 0.5f) - 1.0f;
            r_mouseY = 1.0f - r_ypos / (HEIGHT * 0.5f);
            float dif_z = r_mouseX - temp_xpos;
            temp_xpos = r_mouseX;
            temp_ypos = r_mouseY;
            float senstivity = 30.0;
            dir_z_mouse_move = dif_z * senstivity;
            if (dir_z_mouse_move != 0)
                dir_z_move_flag = true; //xy-plane
        }

        else if (dir_yz_min_distance < 0.0005 && dir_yz_min_distance < dir_xz_min_distance && dir_yz_min_distance < dir_xy_min_distance) //yz平面
        {
            double r_xpos, r_ypos;
            glfwGetCursorPos(window, &r_xpos, &r_ypos);
            r_mouseX = r_xpos / (WIDTH * 0.5f) - 1.0f;
            r_mouseY = 1.0f - r_ypos / (HEIGHT * 0.5f);
            float dif_y = r_mouseX - temp_xpos;
            temp_xpos = r_mouseX;
            temp_ypos = r_mouseY;
            float senstivity = 30.0;
            dir_y_mouse_move = dif_y * senstivity;
            if (dir_y_mouse_move != 0)
                dir_y_move_flag = true; // yz-plane
        }

        if (dir_x_move_flag == true)
        {
            glm::mat4 temp(1.0f);
            temp = glm::rotate(temp, glm::radians(dir_x_mouse_move), dir_y_axis);
            directional_light_direction = glm::vec3(temp * glm::vec4(directional_light_direction, 1.0f));
            dir_circle_model = glm::rotate(dir_circle_model, glm::radians(dir_x_mouse_move), dir_y_axis);
           dir_circle_model1 = glm::rotate(dir_circle_model1, glm::radians(dir_x_mouse_move), dir_y_axis);
            dir_circle_model2 = glm::rotate(dir_circle_model2, glm::radians(dir_x_mouse_move), dir_y_axis);
            dir_total_x_mouse_move += dir_x_mouse_move;
            dir_x_mouse_move = 0.0;
            dir_x_move_flag = false;
            rotate_dir_x_axis = true;
        }

        if (dir_y_move_flag == true)
        {
            glm::mat4 temp(1.0f);
            temp = glm::rotate(temp, glm::radians(dir_y_mouse_move), dir_x_axis);
            directional_light_direction = glm::vec3(temp * glm::vec4(directional_light_direction, 1.0f));
            dir_circle_model = glm::rotate(dir_circle_model, glm::radians(-dir_y_mouse_move), dir_x_axis);
            dir_circle_model1 = glm::rotate(dir_circle_model1, glm::radians(-dir_y_mouse_move), dir_x_axis);
            dir_circle_model2 = glm::rotate(dir_circle_model2, glm::radians(-dir_y_mouse_move), dir_x_axis);
            dir_total_y_mouse_move += -dir_y_mouse_move;
            dir_y_mouse_move = 0.0;
            dir_y_move_flag = false;
            rotate_dir_y_axis = true;
        }

        if (dir_z_move_flag == true)
        {
            glm::mat4 temp(1.0f);
            temp = glm::rotate(temp, glm::radians(dir_z_mouse_move), dir_z_axis);
            directional_light_direction = glm::vec3(temp * glm::vec4(directional_light_direction, 1.0f));
            dir_circle_model = glm::rotate(dir_circle_model, glm::radians(-dir_z_mouse_move), dir_z_axis);
            dir_circle_model1 = glm::rotate(dir_circle_model1, glm::radians(-dir_z_mouse_move), dir_z_axis);
            dir_circle_model2 = glm::rotate(dir_circle_model2, glm::radians(-dir_z_mouse_move), dir_z_axis);
            dir_total_z_mouse_move += -dir_z_mouse_move;
            dir_z_mouse_move = 0.0;
            dir_z_move_flag = false;
            rotate_dir_z_axis = true;
        }

    }

    if (mouse_flag == false)
    {
        if (rotate_self_x_axis == true)
        {
            glm::mat4 temp_matrix(1.0f);
            temp_matrix = glm::rotate(temp_matrix, glm::radians(total_x_mouse_move), self_y_axis);
            self_x_axis = glm::vec3(temp_matrix * glm::vec4(self_x_axis, 1.0f));
            self_z_axis = glm::vec3(temp_matrix * glm::vec4(self_z_axis, 1.0f));
            total_x_mouse_move = 0.0;
            rotate_self_x_axis = false;
        }

        if (rotate_self_y_axis == true)
        {
            glm::mat4 temp_matrix(1.0f);
            temp_matrix = glm::rotate(temp_matrix, glm::radians(total_y_mouse_move), self_x_axis);
            self_y_axis = glm::vec3(temp_matrix * glm::vec4(self_y_axis, 1.0f));
            self_z_axis = glm::vec3(temp_matrix * glm::vec4(self_z_axis, 1.0f));
            total_y_mouse_move = 0.0;
            rotate_self_y_axis = false;
        }

        if (rotate_self_z_axis == true)
        {
            glm::mat4 temp_matrix(1.0f);
            temp_matrix = glm::rotate(temp_matrix, glm::radians(total_z_mouse_move), self_z_axis);
            self_x_axis = glm::vec3(temp_matrix * glm::vec4(self_x_axis, 1.0f));
            self_y_axis = glm::vec3(temp_matrix * glm::vec4(self_y_axis, 1.0f));
            total_z_mouse_move = 0.0;
            rotate_self_z_axis = false;
        }

        if (rotate_dir_x_axis == true)
        {
            glm::mat4 temp_matrix(1.0f);
            temp_matrix = glm::rotate(temp_matrix, glm::radians(dir_total_x_mouse_move), dir_y_axis);
            dir_x_axis = glm::vec3(temp_matrix * glm::vec4(dir_x_axis, 1.0f));
            dir_z_axis = glm::vec3(temp_matrix * glm::vec4(dir_z_axis, 1.0f));
            dir_total_x_mouse_move = 0.0;
            rotate_dir_x_axis = false;
        }

        if (rotate_dir_y_axis == true)
        {
            glm::mat4 temp_matrix(1.0f);
            temp_matrix = glm::rotate(temp_matrix, glm::radians(dir_total_y_mouse_move), dir_x_axis);
            dir_y_axis = glm::vec3(temp_matrix * glm::vec4(dir_y_axis, 1.0f));
            dir_z_axis = glm::vec3(temp_matrix * glm::vec4(dir_z_axis, 1.0f));
            dir_total_y_mouse_move = 0.0;
            rotate_dir_y_axis = false;
        }

        if (rotate_dir_z_axis == true)
        {
            glm::mat4 temp_matrix(1.0f);
            temp_matrix = glm::rotate(temp_matrix, glm::radians(dir_total_z_mouse_move), dir_z_axis);
            dir_x_axis = glm::vec3(temp_matrix * glm::vec4(dir_x_axis, 1.0f));
            dir_y_axis = glm::vec3(temp_matrix * glm::vec4(dir_y_axis, 1.0f));
            dir_total_z_mouse_move = 0.0;
            rotate_dir_z_axis = false;
        }
    }


    if (user_force == true && add_force == false)
    {
        double temp_xpos, temp_ypos;
        glfwGetCursorPos(window, &temp_xpos, &temp_ypos);
        start_xpos = temp_xpos / (WIDTH * 0.5f) - 1.0f;
        start_ypos = 1.0f - temp_ypos / (HEIGHT * 0.5f);
        add_force = true;
    }

    if (user_force == true && add_force == true)
    {
        double user_force_xpos, user_force_ypos;
        glfwGetCursorPos(window, &user_force_xpos, &user_force_ypos);
        uf_xpos = user_force_xpos / (WIDTH * 0.5f) - 1.0f;
        uf_ypos = 1.0f - user_force_ypos / (HEIGHT * 0.5f);
    }

    if (user_force == false && add_force == true)
    {
        double temp_xpos, temp_ypos;
        glfwGetCursorPos(window, &temp_xpos, &temp_ypos);
        end_xpos = temp_xpos / (WIDTH * 0.5f) - 1.0f;
        end_ypos = 1.0f - temp_ypos / (HEIGHT * 0.5f);
        add_force = false;
    }
}


int draw_pickbox(float initial_pickbox_x, float initial_pickbox_y, float temp_pickbox_x, float temp_pickbox_y)
{

    rectangle_vertices.clear();
    rectangle_indices.clear();

    rectangle_vertices.push_back(initial_pickbox_x);
    rectangle_vertices.push_back(initial_pickbox_y);
    rectangle_vertices.push_back(0.1f);
    rectangle_indices.push_back(0);

    rectangle_vertices.push_back(initial_pickbox_x);
    rectangle_vertices.push_back(temp_pickbox_y);
    rectangle_vertices.push_back(0.1f);
    rectangle_indices.push_back(1);

    rectangle_vertices.push_back(temp_pickbox_x);
    rectangle_vertices.push_back(temp_pickbox_y);
    rectangle_vertices.push_back(0.1f);
    rectangle_indices.push_back(2);

    rectangle_vertices.push_back(temp_pickbox_x);
    rectangle_vertices.push_back(initial_pickbox_y);
    rectangle_vertices.push_back(0.1f);
    rectangle_indices.push_back(3);



    GLuint pickbox_VAO, pickbox_VBO, pickbox_EBO;
    glGenVertexArrays(1, &pickbox_VAO);
    glGenBuffers(1, &pickbox_VBO);
    glGenBuffers(1, &pickbox_EBO);

    glBindVertexArray(pickbox_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, pickbox_VBO);
    glBufferData(GL_ARRAY_BUFFER, rectangle_vertices.size() * sizeof(float), &rectangle_vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pickbox_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, rectangle_indices.size() * sizeof(int), &rectangle_indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindVertexArray(0);


    return pickbox_VAO;
}


void drag_objects(GLFWwindow *window)
{
    if (drag_object == true && move == false)
    {
        glfwGetCursorPos(window, &click_object_x, &click_object_y);
        click_object_x = click_object_x / (WIDTH * 0.5f) - 1.0f;
        click_object_y = 1.0f - click_object_y / (HEIGHT * 0.5f);
        if (pl_exist)
        {
            float distance = pow(click_object_x - boxes[pl_info][0], 2) + pow(click_object_y - boxes[pl_info][1], 2);
            float bbox_distance = pow(boxes[pl_info][3], 2) + pow(boxes[pl_info][4], 2);
            if (distance < bbox_distance)
            {
                move = true;
                move_pl = true;
                temp_pos_x = click_object_x;
                temp_pos_y = click_object_y;
            }
        }
    }
    
    if (drag_object == true && move == false)
    {
        glfwGetCursorPos(window, &click_object_x, &click_object_y);
        click_object_x = click_object_x / (WIDTH * 0.5f) - 1.0f;
        click_object_y = 1.0f - click_object_y / (HEIGHT * 0.5f);
        if (object_VAO)
        {
            float distance = pow(click_object_x - boxes[object_info][0], 2) + pow(click_object_y - boxes[object_info][1], 2);
            float bbox_distance = pow(boxes[object_info][3], 2) + pow(boxes[object_info][4], 2);
            if (distance < bbox_distance && xy_min_distance > 0.0005 && xz_min_distance > 0.0005 & yz_min_distance > 0.0005)
            {
                move = true;
                move_object = true;
                temp_pos_x = click_object_x;
                temp_pos_y = click_object_y;
            }
        }
    }

    if (drag_object == true && move_pl == true)
    {
        glm::vec3 x_vec = camera.GetRightVector();
        glm::vec3 y_vec = camera.GetUpVector();
        
        glfwGetCursorPos(window, &new_pos_x, &new_pos_y);

        new_pos_x = new_pos_x / (WIDTH * 0.5f) - 1.0f;
        new_pos_y = 1.0f - new_pos_y / (HEIGHT * 0.5f);
        delta_x = GLfloat(new_pos_x - temp_pos_x)  * 2 * (WIDTH / HEIGHT) * x_vec;
        delta_y = GLfloat(new_pos_y - temp_pos_y)  * 2 * y_vec;
        
        temp_pos_x = new_pos_x;
        temp_pos_y = new_pos_y;
    }
    
    if (drag_object == true && move_object == true)
    {
        glm::vec3 z_vec = glm::normalize(camera.Position);
        glm::vec3 up_vec(0.0f, 1.0f, 0.0f);
        glm::vec3 x_vec = glm::normalize(glm::cross(up_vec, z_vec));
        glm::vec3 y_vec = glm::normalize(glm::cross(z_vec, x_vec));
        glfwGetCursorPos(window, &new_pos_x, &new_pos_y);

        new_pos_x = new_pos_x / (WIDTH * 0.5f) - 1.0f;
        new_pos_y = 1.0f - new_pos_y / (HEIGHT * 0.5f);
        obj_delta_x = GLfloat(new_pos_x - temp_pos_x) * x_vec* (WIDTH / HEIGHT) * 2.2f;
        obj_delta_y = GLfloat(new_pos_y - temp_pos_y) * y_vec * 2.2f;

        temp_pos_x = new_pos_x;
        temp_pos_y = new_pos_y;
    }

    if (drag_object == false)
    {
        move = false;
        move_pl = false;
        move_object = false;
        delta_x = glm::vec3(0.0f, 0.0f, 0.0f);
        delta_y = glm::vec3(0.0f, 0.0f, 0.0f);
        obj_delta_x = glm::vec3(0.0f, 0.0f, 0.0f);
        obj_delta_y = glm::vec3(0.0f, 0.0f, 0.0f);
    }
}


void select_points(double initial_pickbox_x, double initial_pickbox_y, double end_pickbox_x, double end_pickbox_y)
{
    selected_points_indices.clear();

    //物体顶点在屏幕上的坐标
    std::vector<glm::vec3> object_points;
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
    for (int i = 0; i < object_vertices.size() / 3; i++)
    {
        glm::vec4 temp = projection * view * model * glm::vec4(object_vertices[i * 3], object_vertices[i * 3 + 1], object_vertices[i * 3 + 2], 1.0f);
        glm::vec3 point = glm::vec3(temp.x / temp.w, temp.y / temp.w, temp.z / temp.w);
        if (initial_pickbox_x < point.x && point.x < end_pickbox_x && end_pickbox_y < point.y && point.y < initial_pickbox_y)
        {
            glm::vec3 vertex_normal = glm::vec3(object_normals[3 * i], object_normals[3 * i + 1], object_normals[3 * i + 2]);
            glm::vec3 view = glm::normalize(glm::vec3(camera.Position.x - object_vertices[3 * i], camera.Position.y - object_vertices[3 * i + 1], camera.Position.z - object_vertices[3 * i + 2]));
            if (glm::dot(vertex_normal, view) > 0)
                selected_points_indices.push_back(i);
        }
    }
}


//void load_model(std::string nodefile, std::string elemfile, std::vector<glm::vec3>& model_vertices, std::vector<glm::vec3>& model_normals, std::vector<int>& model_indices, std::map<int, std::set<int>>& model_connection)
//{
//    model_vertices.clear();
//    model_normals.clear();
//    model_indices.clear();
//    model_connection.clear();
//    model_vertex_size = 0;
//    model_face_size = 0;
//
//    int unit_num = 0;
//    int tri_per_unit = 0;
//    glm::vec3 Vertex;
//    glm::vec3 Normal;
//    GLfloat Face[3];
//    GLfloat QuaFace[4];
//    std::vector<GLfloat> Vertices;
//    std::vector<glm::vec3> Normals;
//    std::vector<GLuint> Faces;
//    std::vector<float> Tri_areas;
//    std::string Line;
//    std::ifstream File1;
//    std::ifstream File2;
//    std::map<int, std::vector<int>> refer;
//    int line = 0;
//
//    File1.open(nodefile);
//    int line_index = 0;
//    while (std::getline(File1, Line))
//    {
//        if (line_index == 0)
//        {
//            int i = Line.find(" ");
//            model_vertex_size = std::stoi(Line.substr(0, i));
//            line_index++;
//            continue;
//        }
//        else
//        {
//            if (Line == "" || Line[0] == '#')
//                continue;
//            else
//            {
//                sscanf_s(Line.c_str(), "%*f %f %f %f", &Vertex.x, &Vertex.y, &Vertex.z);
//                Vertices.push_back(Vertex.x);
//                Vertices.push_back(Vertex.y);
//                Vertices.push_back(Vertex.z);
//                model_vertices.push_back(Vertex);
//            }
//        }
//    };
//
//    line_index = 0;
//    File2.open(elemfile);
//    while (std::getline(File2, Line))
//    {
//        if (line_index == 0)
//        {
//            int i = Line.find(" ");
//            unit_num = std::stoi(Line.substr(0, i));
//            int j = Line.find(" ", i + 1);
//            tri_per_unit = std::stoi(Line.substr(i + 1, j));
//            model_face_size = unit_num * tri_per_unit;
//            line_index++;
//            continue;
//        }
//        else
//        {
//            if (Line == "" || Line[0] == '#')
//                continue;
//            else
//            {
//                sscanf_s(Line.c_str(), "%*f %f %f %f %*f", &Face[0], &Face[1], &Face[2]);
//                Faces.push_back(Face[0] - 1);
//                Faces.push_back(Face[1] - 1);
//                Faces.push_back(Face[2] - 1);
//                model_indices.push_back(Face[0] - 1);
//                model_indices.push_back(Face[1] - 1);
//                model_indices.push_back(Face[2] - 1);
//                refer[(Face[0] - 1)].push_back(4 * line); //每个顶点所相邻的三角形序号
//                refer[(Face[1] - 1)].push_back(4 * line);
//                refer[(Face[2] - 1)].push_back(4 * line);
//
//                sscanf_s(Line.c_str(), "%*f %f %f %*f %f", &Face[0], &Face[1], &Face[2]);
//                Faces.push_back(Face[0] - 1);
//                Faces.push_back(Face[1] - 1);
//                Faces.push_back(Face[2] - 1);
//                model_indices.push_back(Face[0] - 1);
//                model_indices.push_back(Face[1] - 1);
//                model_indices.push_back(Face[2] - 1);
//                refer[(Face[0] - 1)].push_back(4 * line + 1); //每个顶点所相邻的三角形序号
//                refer[(Face[1] - 1)].push_back(4 * line + 1);
//                refer[(Face[2] - 1)].push_back(4 * line + 1);
//
//                sscanf_s(Line.c_str(), "%*f %f %*f %f %f", &Face[0], &Face[1], &Face[2]);
//                Faces.push_back(Face[0] - 1);
//                Faces.push_back(Face[1] - 1);
//                Faces.push_back(Face[2] - 1);
//                model_indices.push_back(Face[0] - 1);
//                model_indices.push_back(Face[1] - 1);
//                model_indices.push_back(Face[2] - 1);
//                refer[(Face[0] - 1)].push_back(4 * line + 2); //每个顶点所相邻的三角形序号
//                refer[(Face[1] - 1)].push_back(4 * line + 2);
//                refer[(Face[2] - 1)].push_back(4 * line + 2);
//
//                sscanf_s(Line.c_str(), "%*f %*f %f %f %f", &Face[0], &Face[1], &Face[2]);
//                Faces.push_back(Face[0] - 1);
//                Faces.push_back(Face[1] - 1);
//                Faces.push_back(Face[2] - 1);
//                model_indices.push_back(Face[0] - 1);
//                model_indices.push_back(Face[1] - 1);
//                model_indices.push_back(Face[2] - 1);
//                refer[(Face[0] - 1)].push_back(4 * line + 3); //每个顶点所相邻的三角形序号
//                refer[(Face[1] - 1)].push_back(4 * line + 3);
//                refer[(Face[2] - 1)].push_back(4 * line + 3);
//
//                line++;
//                //将每条边存入map中
//                sscanf_s(Line.c_str(), "%*f %f %f %f %f", &QuaFace[0], &QuaFace[1], &QuaFace[2], &QuaFace[3]);
//                std::sort(QuaFace, QuaFace + 4);
//                model_connection[QuaFace[0] - 1].insert(QuaFace[1] - 1);
//                model_connection[QuaFace[0] - 1].insert(QuaFace[2] - 1);
//                model_connection[QuaFace[0] - 1].insert(QuaFace[3] - 1);
//                model_connection[QuaFace[1] - 1].insert(QuaFace[2] - 1);
//                model_connection[QuaFace[1] - 1].insert(QuaFace[3] - 1);
//                model_connection[QuaFace[2] - 1].insert(QuaFace[3] - 1);
//            }
//
//            //else
//            //{
//            //    sscanf_s(Line.c_str(), "%*f %f %f %f %*f", &Face[0], &Face[1], &Face[2]);
//            //    Faces.push_back(Face[0]);
//            //    Faces.push_back(Face[1]);
//            //    Faces.push_back(Face[2]);
//            //    model_indices.push_back(Face[0]);
//            //    model_indices.push_back(Face[1]);
//            //    model_indices.push_back(Face[2]);
//            //    refer[(Face[0])].push_back(4 * line); //每个顶点所相邻的三角形序号
//            //    refer[(Face[1])].push_back(4 * line);
//            //    refer[(Face[2])].push_back(4 * line);
//
//            //    sscanf_s(Line.c_str(), "%*f %f %f %*f %f", &Face[0], &Face[1], &Face[2]);
//            //    Faces.push_back(Face[0]);
//            //    Faces.push_back(Face[1]);
//            //    Faces.push_back(Face[2]);
//            //    model_indices.push_back(Face[0]);
//            //    model_indices.push_back(Face[1]);
//            //    model_indices.push_back(Face[2]);
//            //    refer[(Face[0])].push_back(4 * line + 1); //每个顶点所相邻的三角形序号
//            //    refer[(Face[1])].push_back(4 * line + 1);
//            //    refer[(Face[2])].push_back(4 * line + 1);
//
//            //    sscanf_s(Line.c_str(), "%*f %f %*f %f %f", &Face[0], &Face[1], &Face[2]);
//            //    Faces.push_back(Face[0]);
//            //    Faces.push_back(Face[1]);
//            //    Faces.push_back(Face[2]);
//            //    model_indices.push_back(Face[0]);
//            //    model_indices.push_back(Face[1]);
//            //    model_indices.push_back(Face[2]);
//            //    refer[(Face[0])].push_back(4 * line + 2); //每个顶点所相邻的三角形序号
//            //    refer[(Face[1])].push_back(4 * line + 2);
//            //    refer[(Face[2])].push_back(4 * line + 2);
//
//            //    sscanf_s(Line.c_str(), "%*f %*f %f %f %f", &Face[0], &Face[1], &Face[2]);
//            //    Faces.push_back(Face[0]);
//            //    Faces.push_back(Face[1]);
//            //    Faces.push_back(Face[2]);
//            //    model_indices.push_back(Face[0]);
//            //    model_indices.push_back(Face[1]);
//            //    model_indices.push_back(Face[2]);
//            //    refer[(Face[0])].push_back(4 * line + 3); //每个顶点所相邻的三角形序号
//            //    refer[(Face[1])].push_back(4 * line + 3);
//            //    refer[(Face[2])].push_back(4 * line + 3);
//
//            //    line++;
//            //    //将每条边存入map中
//            //    sscanf_s(Line.c_str(), "%*f %f %f %f %f", &QuaFace[0], &QuaFace[1], &QuaFace[2], &QuaFace[3]);
//            //    std::sort(QuaFace, QuaFace + 4);
//            //    model_connection[QuaFace[0]].insert(QuaFace[1]);
//            //    model_connection[QuaFace[0] ].insert(QuaFace[2]);
//            //    model_connection[QuaFace[0] ].insert(QuaFace[3] );
//            //    model_connection[QuaFace[1]].insert(QuaFace[2] );
//            //    model_connection[QuaFace[1] ].insert(QuaFace[3]);
//            //    model_connection[QuaFace[2] ].insert(QuaFace[3] );
//            //}
//        }
//    }
//
//
//    //计算每个面的法向量和面积
//    for (int i = 0; i < unit_num; i++)
//    {
//        for (int j = 0; j < tri_per_unit; j++)
//        {
//            glm::vec3 p1(Vertices[3 * Faces[12 * i + 3 * j]], Vertices[3 * Faces[12 * i + 3 * j] + 1], Vertices[3 * Faces[12 * i + 3 * j] + 2]);
//            glm::vec3 p2(Vertices[3 * Faces[12 * i + 3 * j + 1]], Vertices[3 * Faces[12 * i + 3 * j + 1] + 1], Vertices[3 * Faces[12 * i + 3 * j + 1] + 2]);
//            glm::vec3 p3(Vertices[3 * Faces[12 * i + 3 * j + 2]], Vertices[3 * Faces[12 * i + 3 * j + 2] + 1], Vertices[3 * Faces[12 * i + 3 * j + 2] + 2]);
//            glm::vec3 normal = glm::normalize(glm::cross(glm::normalize(p2 - p1), glm::normalize(p3 - p1)));
//            Normals.push_back(normal); //Normals.size() == Faces.size()
//            float area = glm::length(glm::cross(p2 - p1, p3 - p1)) / 2.0;
//            Tri_areas.push_back(area); // area of triangles
//        }
//    }
//    int a = 0;
//    glm::vec3 vertex_normal;
//    glm::vec3 sum_weighted_normal(0.0f);
//    for (int i = 0; i < model_vertex_size; i++)
//    {
//        sum_weighted_normal = glm::vec3(0.0f, 0.0f, 0.0f);
//        for (auto iter = refer[i].cbegin(); iter != refer[i].cend(); iter++)
//        {
//            sum_weighted_normal = sum_weighted_normal + Tri_areas[*iter] * Normals[*iter];
//        }
//        vertex_normal = glm::normalize(sum_weighted_normal);
//        model_normals.push_back(vertex_normal);
//    }
//}

glm::mat4 BuildRotationMatrix(glm::vec3 start_vec, glm::vec3 end_vec)
{
    glm::vec3 v = glm::cross(start_vec, end_vec);
    float s = glm::length(v);
    float c = glm::dot(start_vec, end_vec);
    float cef = 1 / (1 + c);
    glm::mat3 I(1.0f);
    float Vx[9] = {
        0, -v.z, v.y,
        v.z, 0, -v.x,
        -v.y, v.x, 0
    };
    glm::mat3 V_x = glm::make_mat3(Vx);
    glm::mat3 V_x_pow2 = V_x * V_x;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            V_x_pow2[i][j] *= cef;
    glm::mat3 temp_R = I + V_x + V_x_pow2;
    float R_elm[16] = {
        temp_R[0][0], temp_R[1][0], temp_R[2][0], 0,
        temp_R[0][1], temp_R[1][1], temp_R[2][1], 0,
        temp_R[0][2], temp_R[1][2], temp_R[2][2], 0,
        0, 0, 0, 1
    };
    glm::mat4 R = glm::make_mat4(R_elm);
    return R;
}

void load_model(std::string nodefile, std::string elemfile, std::vector<glm::vec3>& model_vertices, std::vector<glm::vec3>& model_normals, std::vector<int>& model_indices, std::map<int, std::set<int>>& model_connection)
{
    model_vertices.clear();
    model_normals.clear();
    model_indices.clear();
    model_connection.clear();
    model_vertex_size = 0;
    model_face_size = 0;

    int unit_num = 0;
    int tri_per_unit = 0;
    glm::vec3 Vertex;
    glm::vec3 Normal;
    GLfloat Face[3];
    GLfloat QuaFace[4];
    std::vector<GLfloat> Vertices;
    std::vector<glm::vec3> Normals;
    std::vector<GLuint> Faces;
    std::vector<float> Tri_areas;
    std::string Line;
    std::ifstream File1;
    std::ifstream File2;
    std::map<int, std::vector<int>> refer;
    int line = 0;

    double x_max = -999.0, y_max = -999.0, z_max = -999.0;
    double x_min = 999.0, y_min = 999.0, z_min = 999.0;
    double x_range = 0.0, y_range = 0.0, z_range = 0.0;
    double yx_ratio = 0.0, zx_ratio = 0.0;
    File1.open(nodefile);
    int line_index = 0;
    while (std::getline(File1, Line))
    {
        if (line_index == 0)
        {
            int i = Line.find(" ");
            model_vertex_size = std::stoi(Line.substr(0, i));
            line_index++;
            continue;
        }
        else
        {
            if (Line == "" || Line[0] == '#')
                continue;
            else
            {
                sscanf_s(Line.c_str(), "%*f %f %f %f", &Vertex.x, &Vertex.y, &Vertex.z);
                Vertices.push_back(Vertex.x);
                Vertices.push_back(Vertex.y);
                Vertices.push_back(Vertex.z);
                model_vertices.push_back(Vertex);

                if (Vertex.x < x_min)
                    x_min = Vertex.x;
                else if (Vertex.x > x_max)
                    x_max = Vertex.x;

                if (Vertex.y < y_min)
                    y_min = Vertex.y;
                else if (Vertex.y > y_max)
                    y_max = Vertex.y;

                if (Vertex.z < z_min)
                    z_min = Vertex.z;
                else if (Vertex.z > z_max)
                    z_max = Vertex.z;
            }
        }
    };
    x_range = (x_max - x_min) / 2.0;
    y_range = (y_max - y_min) / 2.0;
    z_range = (z_max - z_min) / 2.0;
    yx_ratio = y_range / x_range;
    zx_ratio = z_range / x_range;
    for (int i = 0; i < model_vertex_size; i++)
        model_vertices[i] = glm::vec3(model_vertices[i].x / x_range, model_vertices[i].y * yx_ratio/ y_range , model_vertices[i].z * zx_ratio/ z_range);


    line_index = 0;
    File2.open(elemfile);
    while (std::getline(File2, Line))
    {
        if (line_index == 0)
        {
            int i = Line.find(" ");
            unit_num = std::stoi(Line.substr(0, i));
            int j = Line.find(" ", i + 1);
            tri_per_unit = std::stoi(Line.substr(i + 1, j));
            model_face_size = unit_num * tri_per_unit;
            line_index++;
            continue;
        }
        else
        {
            if (Line == "" || Line[0] == '#')
                continue;
            else
            {
                sscanf_s(Line.c_str(), "%*f %f %f %f %*f", &Face[0], &Face[1], &Face[2]);
                Faces.push_back(Face[0] - 1);
                Faces.push_back(Face[1] - 1);
                Faces.push_back(Face[2] - 1);
                model_indices.push_back(Face[0] - 1);
                model_indices.push_back(Face[1] - 1);
                model_indices.push_back(Face[2] - 1);
                refer[(Face[0] - 1)].push_back(4 * line); //每个顶点所相邻的三角形序号
                refer[(Face[1] - 1)].push_back(4 * line);
                refer[(Face[2] - 1)].push_back(4 * line);

                sscanf_s(Line.c_str(), "%*f %f %f %*f %f", &Face[0], &Face[1], &Face[2]);
                Faces.push_back(Face[0] - 1);
                Faces.push_back(Face[1] - 1);
                Faces.push_back(Face[2] - 1);
                model_indices.push_back(Face[0] - 1);
                model_indices.push_back(Face[1] - 1);
                model_indices.push_back(Face[2] - 1);
                refer[(Face[0] - 1)].push_back(4 * line + 1); //每个顶点所相邻的三角形序号
                refer[(Face[1] - 1)].push_back(4 * line + 1);
                refer[(Face[2] - 1)].push_back(4 * line + 1);

                sscanf_s(Line.c_str(), "%*f %f %*f %f %f", &Face[0], &Face[1], &Face[2]);
                Faces.push_back(Face[0] - 1);
                Faces.push_back(Face[1] - 1);
                Faces.push_back(Face[2] - 1);
                model_indices.push_back(Face[0] - 1);
                model_indices.push_back(Face[1] - 1);
                model_indices.push_back(Face[2] - 1);
                refer[(Face[0] - 1)].push_back(4 * line + 2); //每个顶点所相邻的三角形序号
                refer[(Face[1] - 1)].push_back(4 * line + 2);
                refer[(Face[2] - 1)].push_back(4 * line + 2);

                sscanf_s(Line.c_str(), "%*f %*f %f %f %f", &Face[0], &Face[1], &Face[2]);
                Faces.push_back(Face[0] - 1);
                Faces.push_back(Face[1] - 1);
                Faces.push_back(Face[2] - 1);
                model_indices.push_back(Face[0] - 1);
                model_indices.push_back(Face[1] - 1);
                model_indices.push_back(Face[2] - 1);
                refer[(Face[0] - 1)].push_back(4 * line + 3); //每个顶点所相邻的三角形序号
                refer[(Face[1] - 1)].push_back(4 * line + 3);
                refer[(Face[2] - 1)].push_back(4 * line + 3);

                line++;
                //将每条边存入map中
                sscanf_s(Line.c_str(), "%*f %f %f %f %f", &QuaFace[0], &QuaFace[1], &QuaFace[2], &QuaFace[3]);
                std::sort(QuaFace, QuaFace + 4);
                model_connection[QuaFace[0] - 1].insert(QuaFace[1] - 1);
                model_connection[QuaFace[0] - 1].insert(QuaFace[2] - 1);
                model_connection[QuaFace[0] - 1].insert(QuaFace[3] - 1);
                model_connection[QuaFace[1] - 1].insert(QuaFace[2] - 1);
                model_connection[QuaFace[1] - 1].insert(QuaFace[3] - 1);
                model_connection[QuaFace[2] - 1].insert(QuaFace[3] - 1);
            }

            //else
            //{
            //    sscanf_s(Line.c_str(), "%*f %f %f %f %*f", &Face[0], &Face[1], &Face[2]);
            //    Faces.push_back(Face[0]);
            //    Faces.push_back(Face[1]);
            //    Faces.push_back(Face[2]);
            //    model_indices.push_back(Face[0]);
            //    model_indices.push_back(Face[1]);
            //    model_indices.push_back(Face[2]);
            //    refer[(Face[0])].push_back(4 * line); //每个顶点所相邻的三角形序号
            //    refer[(Face[1])].push_back(4 * line);
            //    refer[(Face[2])].push_back(4 * line);

            //    sscanf_s(Line.c_str(), "%*f %f %f %*f %f", &Face[0], &Face[1], &Face[2]);
            //    Faces.push_back(Face[0]);
            //    Faces.push_back(Face[1]);
            //    Faces.push_back(Face[2]);
            //    model_indices.push_back(Face[0]);
            //    model_indices.push_back(Face[1]);
            //    model_indices.push_back(Face[2]);
            //    refer[(Face[0])].push_back(4 * line + 1); //每个顶点所相邻的三角形序号
            //    refer[(Face[1])].push_back(4 * line + 1);
            //    refer[(Face[2])].push_back(4 * line + 1);

            //    sscanf_s(Line.c_str(), "%*f %f %*f %f %f", &Face[0], &Face[1], &Face[2]);
            //    Faces.push_back(Face[0]);
            //    Faces.push_back(Face[1]);
            //    Faces.push_back(Face[2]);
            //    model_indices.push_back(Face[0]);
            //    model_indices.push_back(Face[1]);
            //    model_indices.push_back(Face[2]);
            //    refer[(Face[0])].push_back(4 * line + 2); //每个顶点所相邻的三角形序号
            //    refer[(Face[1])].push_back(4 * line + 2);
            //    refer[(Face[2])].push_back(4 * line + 2);

            //    sscanf_s(Line.c_str(), "%*f %*f %f %f %f", &Face[0], &Face[1], &Face[2]);
            //    Faces.push_back(Face[0]);
            //    Faces.push_back(Face[1]);
            //    Faces.push_back(Face[2]);
            //    model_indices.push_back(Face[0]);
            //    model_indices.push_back(Face[1]);
            //    model_indices.push_back(Face[2]);
            //    refer[(Face[0])].push_back(4 * line + 3); //每个顶点所相邻的三角形序号
            //    refer[(Face[1])].push_back(4 * line + 3);
            //    refer[(Face[2])].push_back(4 * line + 3);

            //    line++;
            //    //将每条边存入map中
            //    sscanf_s(Line.c_str(), "%*f %f %f %f %f", &QuaFace[0], &QuaFace[1], &QuaFace[2], &QuaFace[3]);
            //    std::sort(QuaFace, QuaFace + 4);
            //    model_connection[QuaFace[0]].insert(QuaFace[1]);
            //    model_connection[QuaFace[0] ].insert(QuaFace[2]);
            //    model_connection[QuaFace[0] ].insert(QuaFace[3] );
            //    model_connection[QuaFace[1]].insert(QuaFace[2] );
            //    model_connection[QuaFace[1] ].insert(QuaFace[3]);
            //    model_connection[QuaFace[2] ].insert(QuaFace[3] );
            //}
        }
    }


    //计算每个面的法向量和面积
    for (int i = 0; i < unit_num; i++)
    {
        for (int j = 0; j < tri_per_unit; j++)
        {
            glm::vec3 p1(Vertices[3 * Faces[12 * i + 3 * j]], Vertices[3 * Faces[12 * i + 3 * j] + 1], Vertices[3 * Faces[12 * i + 3 * j] + 2]);
            glm::vec3 p2(Vertices[3 * Faces[12 * i + 3 * j + 1]], Vertices[3 * Faces[12 * i + 3 * j + 1] + 1], Vertices[3 * Faces[12 * i + 3 * j + 1] + 2]);
            glm::vec3 p3(Vertices[3 * Faces[12 * i + 3 * j + 2]], Vertices[3 * Faces[12 * i + 3 * j + 2] + 1], Vertices[3 * Faces[12 * i + 3 * j + 2] + 2]);
            glm::vec3 normal = glm::normalize(glm::cross(glm::normalize(p2 - p1), glm::normalize(p3 - p1)));
            Normals.push_back(normal); //Normals.size() == Faces.size()
            float area = glm::length(glm::cross(p2 - p1, p3 - p1)) / 2.0;
            Tri_areas.push_back(area); // area of triangles
        }
    }
    int a = 0;
    glm::vec3 vertex_normal;
    glm::vec3 sum_weighted_normal(0.0f);
    for (int i = 0; i < model_vertex_size; i++)
    {
        sum_weighted_normal = glm::vec3(0.0f, 0.0f, 0.0f);
        for (auto iter = refer[i].cbegin(); iter != refer[i].cend(); iter++)
        {
            sum_weighted_normal = sum_weighted_normal + Tri_areas[*iter] * Normals[*iter];
        }
        vertex_normal = glm::normalize(sum_weighted_normal);
        model_normals.push_back(vertex_normal);
    }
}