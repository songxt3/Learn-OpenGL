#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <math.h>

using namespace std;

int width = 1000;
int height = 720;

void reset(float& am_factor, float& sp_factor,float& di_factor,int& sh_factor) {
    am_factor = 0.1f;
    sp_factor = 0.5f;
    di_factor = 1.0f;
    sh_factor = 16;
}

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"out vec3 FragPos;\n"
"out vec3 Normal;\n"
"out vec3 OurColor;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform vec3 lightPos;\n"
"uniform vec3 viewPos;\n"
"uniform vec3 lightColor;\n"
"uniform vec3 objectColor;\n"
"uniform float ambient_factor;\n"
"uniform float specular_factor;\n"
"uniform float diffuse_factor;\n"
"uniform float shininess_factor;\n"
"uniform int choose;\n"
"void main()\n"
"{\n"
"   if (choose == 1) {\n"
"       gl_Position = projection * view * model * vec4(aPos,1.0);\n"
"       FragPos = vec3(model * vec4(aPos, 1.0));\n"
"       Normal = aNormal;\n"
"   } else {\n"
"       gl_Position = projection * view * model * vec4(aPos,1.0);\n"
"       vec3 Position = vec3(model * vec4(aPos, 1.0));\n"
"       vec3 Normal_ = mat3(transpose(inverse(model))) * aNormal;\n"
"       vec3 norm = normalize(Normal_);\n"
"       vec3 ambient = ambient_factor * lightColor;\n"
"       vec3 lightDir = normalize(lightPos - Position);\n"
"       float diff = max(dot(norm, lightDir), 0.0);\n"
"       vec3 diffuse =  diffuse_factor * diff * lightColor;\n"
"       vec3 viewDir = normalize(viewPos - Position);\n"
"       vec3 reflectDir = reflect(-lightDir, norm);\n"
"       float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess_factor);\n"
"       vec3 specular = specular_factor * spec * lightColor;\n"
"       OurColor = (ambient + diffuse + specular) * objectColor;\n"
"   }\n"
"}\0";

const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 Normal;\n"
"in vec3 FragPos;\n"
"in vec3 OurColor;\n"
"uniform vec3 lightPos;\n"
"uniform vec3 viewPos;\n"
"uniform vec3 lightColor;\n"
"uniform vec3 objectColor;\n"
"uniform float ambient_factor;\n"
"uniform float specular_factor;\n"
"uniform float diffuse_factor;\n"
"uniform float shininess_factor;\n"
"uniform int choose;\n"
"void main()\n"
"{\n"
"   if (choose == 1) {\n"
"       vec3 ambient = ambient_factor * lightColor;\n"
"       vec3 norm = normalize(Normal);\n"
"       vec3 lightDir = normalize(lightPos - FragPos);\n"
"       float diff = max(dot(norm, lightDir), 0.0);\n"
"       vec3 diffuse = diffuse_factor * diff * lightColor;\n"
"       vec3 viewDir = normalize(viewPos - FragPos);\n"
"       vec3 reflectDir = reflect(-lightDir, norm);\n"
"       float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess_factor);\n"
"       vec3 specular = specular_factor * spec * lightColor;\n"
"       vec3 result = (ambient + diffuse + specular) * objectColor;\n"
"       FragColor = vec4(result, 1.0);\n"
"   } else {\n"
"       FragColor = vec4(OurColor, 1.0);\n"
"   }\n"
"}\n\0";


int main(int, char**) {
    // initialize
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(width, height, "CG hw1", NULL, NULL);
    glfwMakeContextCurrent(window);
    
    //initial glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // Setup ImGui binding
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfwGL3_Init(window, true);
    
    glEnable(GL_DEPTH_TEST);
    //    glDisable(GL_DEPTH_TEST);
    float vertices[] = {
        -0.2f, -0.2f, -0.2f,  0.0f,  0.0f, -1.0f,
        0.2f, -0.2f, -0.2f,  0.0f,  0.0f, -1.0f,
        0.2f,  0.2f, -0.2f,  0.0f,  0.0f, -1.0f,
        0.2f,  0.2f, -0.2f,  0.0f,  0.0f, -1.0f,
        -0.2f,  0.2f, -0.2f,  0.0f,  0.0f, -1.0f,
        -0.2f, -0.2f, -0.2f,  0.0f,  0.0f, -1.0f,
        
        -0.2f, -0.2f,  0.2f,  0.0f,  0.0f,  1.0f,
        0.2f, -0.2f,  0.2f,  0.0f,  0.0f,  1.0f,
        0.2f,  0.2f,  0.2f,  0.0f,  0.0f,  1.0f,
        0.2f,  0.2f,  0.2f,  0.0f,  0.0f,  1.0f,
        -0.2f,  0.2f,  0.2f,  0.0f,  0.0f,  1.0f,
        -0.2f, -0.2f,  0.2f,  0.0f,  0.0f,  1.0f,
        
        -0.2f,  0.2f,  0.2f, -1.0f,  0.0f,  0.0f,
        -0.2f,  0.2f, -0.2f, -1.0f,  0.0f,  0.0f,
        -0.2f, -0.2f, -0.2f, -1.0f,  0.0f,  0.0f,
        -0.2f, -0.2f, -0.2f, -1.0f,  0.0f,  0.0f,
        -0.2f, -0.2f,  0.2f, -1.0f,  0.0f,  0.0f,
        -0.2f,  0.2f,  0.2f, -1.0f,  0.0f,  0.0f,
        
        0.2f,  0.2f,  0.2f,  1.0f,  0.0f,  0.0f,
        0.2f,  0.2f, -0.2f,  1.0f,  0.0f,  0.0f,
        0.2f, -0.2f, -0.2f,  1.0f,  0.0f,  0.0f,
        0.2f, -0.2f, -0.2f,  1.0f,  0.0f,  0.0f,
        0.2f, -0.2f,  0.2f,  1.0f,  0.0f,  0.0f,
        0.2f,  0.2f,  0.2f,  1.0f,  0.0f,  0.0f,
        
        -0.2f, -0.2f, -0.2f,  0.0f, -1.0f,  0.0f,
        0.2f, -0.2f, -0.2f,  0.0f, -1.0f,  0.0f,
        0.2f, -0.2f,  0.2f,  0.0f, -1.0f,  0.0f,
        0.2f, -0.2f,  0.2f,  0.0f, -1.0f,  0.0f,
        -0.2f, -0.2f,  0.2f,  0.0f, -1.0f,  0.0f,
        -0.2f, -0.2f, -0.2f,  0.0f, -1.0f,  0.0f,
        
        -0.2f,  0.2f, -0.2f,  0.0f,  1.0f,  0.0f,
        0.2f,  0.2f, -0.2f,  0.0f,  1.0f,  0.0f,
        0.2f,  0.2f,  0.2f,  0.0f,  1.0f,  0.0f,
        0.2f,  0.2f,  0.2f,  0.0f,  1.0f,  0.0f,
        -0.2f,  0.2f,  0.2f,  0.0f,  1.0f,  0.0f,
        -0.2f,  0.2f, -0.2f,  0.0f,  1.0f,  0.0f
    };
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindVertexArray(VAO);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    glUseProgram(shaderProgram);
    
    ImGui::StyleColorsLight();
    
    //set para
    bool show_window = true;
    bool Phong = false;
    bool Gouraud = false;
    bool bonus = false;
    static float am_factor = 0.1f;
    static float sp_factor = 0.5f;
    static float di_factor = 1.0f;
    static int sh_factor = 16;
    int choose = 1;
    
    // render loop
    while (!glfwWindowShouldClose(window)) {
        ImGui_ImplGlfwGL3_NewFrame();
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        unsigned int choose_part = glGetUniformLocation(shaderProgram, "choose");
        glUniform1i(choose_part, choose);
        
        if (show_window) {
            ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiSetCond_Once);
            ImGui::Begin("Color Tool", &show_window, ImGuiWindowFlags_MenuBar);
            ImGui::Text("Hello");
            ImGui::Text("Choose the different factors");
            ImGui::SliderFloat("ambient factor", &am_factor, 0.1f, 1.5f);
            ImGui::SliderFloat("specular factor", &sp_factor, 0.1f, 100.0f);
            ImGui::SliderFloat("diffuse factor", &di_factor, 0.1f, 3.0f);
            ImGui::SliderInt("shininess factor", &sh_factor, 1, 300);
            if (ImGui::CollapsingHeader("Phong Shading")) {
                if (ImGui::Button("Start Phong")) {
                    reset(am_factor, sp_factor, di_factor, sh_factor);
                    choose = 1;
                    Phong = true;
                    Gouraud = false;
                    bonus = false;
                }
            }
            if (ImGui::CollapsingHeader("Gouraud Shading")) {
                if (ImGui::Button("Start Gouraud")) {
                    reset(am_factor, sp_factor, di_factor, sh_factor);
                    sp_factor = 8.0f;
                    choose = 0;
                    Phong = false;
                    Gouraud = true;
                    bonus = false;
                }
            }
            if (ImGui::CollapsingHeader("Bonus")) {
                if (ImGui::Button("Start")) {
                    reset(am_factor, sp_factor, di_factor, sh_factor);
                    choose = 1;
                    Phong = false;
                    Gouraud = false;
                    bonus = true;
                }
            }
            ImGui::End();
        }
        
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
        glm::vec3 objectColor, lightColor, lightPos, viewPos;
      
        glUseProgram(shaderProgram);
        if (Phong) {
            objectColor = glm::vec3(0.902f, 0.0f, 0.0f);
            lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
            lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
            viewPos = glm::vec3(0.0f, 0.0f, 3.0f);
            view = glm::lookAt(glm::vec3(2.0, 2.0, 2.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
            projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
        } else if (Gouraud) {
            objectColor = glm::vec3(0.902f, 0.0f, 0.0f);
            lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
            lightPos = glm::vec3(-1.0f, 1.0f, 2.0f);
            viewPos = glm::vec3(0.0f, 0.0f, 3.0f);
            view = glm::lookAt(glm::vec3(2.0, 2.0, 2.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
            projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
        } else if (bonus) {
            lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
            lightPos.y = sin(glfwGetTime());
            lightPos.z = 1.0f;
            objectColor = glm::vec3(0.902f, 0.0f, 0.0f);
            lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
            viewPos = glm::vec3(0.0f, 0.0f, 3.0f);
            view = glm::lookAt(glm::vec3(2.0, 2.0, 2.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
            projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
        } else {
            view = glm::lookAt(glm::vec3(2.0, 2.0, 2.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
            projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
        }
        
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc  = glGetUniformLocation(shaderProgram, "view");
        unsigned int project  = glGetUniformLocation(shaderProgram, "projection");
        
        unsigned int object = glGetUniformLocation(shaderProgram, "objectColor");
        unsigned int lightC  = glGetUniformLocation(shaderProgram, "lightColor");
        unsigned int lightP  = glGetUniformLocation(shaderProgram, "lightPos");
        unsigned int viewP = glGetUniformLocation(shaderProgram, "viewPos");
        
        unsigned int ambient_part = glGetUniformLocation(shaderProgram, "ambient_factor");
        unsigned int specular_part = glGetUniformLocation(shaderProgram, "specular_factor");
        unsigned int diffuse_part = glGetUniformLocation(shaderProgram, "diffuse_factor");
        unsigned int shininess_part = glGetUniformLocation(shaderProgram, "shininess_factor");
        
        glUniform1f(ambient_part, am_factor);
        glUniform1f(specular_part, sp_factor);
        glUniform1f(diffuse_part, di_factor);
        glUniform1f(shininess_part, sh_factor);
        
        glUniform3fv(object, 1, glm::value_ptr(objectColor));
        glUniform3fv(lightC, 1, glm::value_ptr(lightColor));
        glUniform3fv(lightP, 1, glm::value_ptr(lightPos));
        glUniform3fv(viewP, 1, glm::value_ptr(viewPos));
        
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(project, 1, GL_FALSE, glm::value_ptr(projection));
        
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        
        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        
        glfwPollEvents();
    }
    
    // cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    
    return 0;
}

