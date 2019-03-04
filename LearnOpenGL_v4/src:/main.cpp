#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include "Camera.hpp"
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

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
float x_pos, y_pos;

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(aPos,1.0);\n"
"   ourColor = aColor;\n"
"}\0";

const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, 0.5f);\n"
"}\n\0";

int main(int, char**) {
    // initialize
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(width, height, "CG hw", NULL, NULL);
    glfwMakeContextCurrent(window);

    
    glfwSetCursorPosCallback(window, mouse_callback);
    
    
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
    
    float vertices[] = {
        -0.2f, -0.2f, -0.2f, 1.0f, 0.0f, 0.0f,
        0.2f, -0.2f, -0.2f, 1.0f, 0.0f, 0.0f,
        0.2f,  0.2f, -0.2f, 1.0f, 0.0f, 0.0f,
        0.2f,  0.2f, -0.2f, 1.0f, 0.0f, 0.0f,
        -0.2f,  0.2f, -0.2f, 1.0f, 0.0f, 0.0f,
        -0.2f, -0.2f, -0.2f, 1.0f, 0.0f, 0.0f,
        
        -0.2f, -0.2f,  0.2f, 0.0f, 1.0f, 0.0f,
        0.2f, -0.2f,  0.2f, 0.0f, 1.0f, 0.0f,
        0.2f,  0.2f,  0.2f, 0.0f, 1.0f, 0.0f,
        0.2f,  0.2f,  0.2f, 0.0f, 1.0f, 0.0f,
        -0.2f,  0.2f,  0.2f, 0.0f, 1.0f, 0.0f,
        -0.2f, -0.2f,  0.2f, 0.0f, 1.0f, 0.0f,
        
        -0.2f,  0.2f,  0.2f, 0.0f, 0.0f, 1.0f,
        -0.2f,  0.2f, -0.2f, 0.0f, 0.0f, 1.0f,
        -0.2f, -0.2f, -0.2f, 0.0f, 0.0f, 1.0f,
        -0.2f, -0.2f, -0.2f, 0.0f, 0.0f, 1.0f,
        -0.2f, -0.2f,  0.2f, 0.0f, 0.0f, 1.0f,
        -0.2f,  0.2f,  0.2f, 0.0f, 0.0f, 1.0f,
        
        0.2f,  0.2f,  0.2f, 1.0f, 1.0f, 0.0f,
        0.2f,  0.2f, -0.2f, 1.0f, 1.0f, 0.0f,
        0.2f, -0.2f, -0.2f, 1.0f, 1.0f, 0.0f,
        0.2f, -0.2f, -0.2f, 1.0f, 1.0f, 0.0f,
        0.2f, -0.2f,  0.2f, 1.0f, 1.0f, 0.0f,
        0.2f,  0.2f,  0.2f, 1.0f, 1.0f, 0.0f,
        
        -0.2f, -0.2f, -0.2f, 1.0f, 0.0f, 1.0f,
        0.2f, -0.2f, -0.2f, 1.0f, 0.0f, 1.0f,
        0.2f, -0.2f,  0.2f, 1.0f, 0.0f, 1.0f,
        0.2f, -0.2f,  0.2f, 1.0f, 0.0f, 1.0f,
        -0.2f, -0.2f,  0.2f, 1.0f, 0.0f, 1.0f,
        -0.2f, -0.2f, -0.2f, 1.0f, 0.0f, 1.0f,
        
        -0.2f,  0.2f, -0.2f, 0.0f, 1.0f, 1.0f,
        0.2f,  0.2f, -0.2f, 0.0f, 1.0f, 1.0f,
        0.2f,  0.2f,  0.2f, 0.0f, 1.0f, 1.0f,
        0.2f,  0.2f,  0.2f,  0.0f, 1.0f, 1.0f,
        -0.2f,  0.2f,  0.2f,  0.0f, 1.0f, 1.0f,
        -0.2f,  0.2f, -0.2f, 0.0f, 1.0f, 1.0f
    };
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
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
    bool orthographic = false;
    bool perspective = false;
    bool show_window = true;
    bool View = false;
    bool bouns_key = false;
    float trans_dis = 0.0f;
    glm::vec3 center = glm::vec3(-1.5, 0.5, -1.5);
    Camera key_control(cameraPos, cameraFront, cameraUp, 0.05f);
    Camera mouse_control(cameraPos, cameraFront, cameraUp, 0.05f);
    // render loop
    while (!glfwWindowShouldClose(window)) {
        ImGui_ImplGlfwGL3_NewFrame();
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
        
        if (show_window) {
            ImGui::SetNextWindowSize(ImVec2(400, 250), ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2(520, 50), ImGuiSetCond_Once);
            ImGui::Begin("Color Tool", &show_window, ImGuiWindowFlags_MenuBar);
            ImGui::Text("Hello");
            ImGui::Text("Choose the operation");
            if (ImGui::CollapsingHeader("orthographic projection")) {
                if (ImGui::Button("Start orthographic")) {
                    orthographic = true;
                    perspective = View = false;
                    bouns_key = false;
                }
            }
            if (ImGui::CollapsingHeader("perspective projection")) {
                if (ImGui::Button("Start perspective")) {
                    orthographic = View = false;
                    perspective = true;
                    bouns_key = false;
                }
            }
            if (ImGui::CollapsingHeader("View Changing")) {
                if (ImGui::Button("Start View")) {
                    orthographic = perspective = false;
                    View = true;
                    bouns_key = false;
                }
            }
            if (ImGui::CollapsingHeader("Bonus")) {
                ImGui::Text("Press ESC to exit the window~~~");
                if (ImGui::Button("start FPS camera")) {
                    key_control = Camera(cameraPos, cameraFront, cameraUp, 0.05f);
                    orthographic = perspective = View = false;
                    bouns_key = true;
                }
            }
            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("Reset")) {
                    orthographic = perspective = View = false;
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            ImGui::End();
        }

        
        glUseProgram(shaderProgram);

        if (orthographic) {
            model = glm::mat4(1.0f);
            projection = glm::mat4(1.0f);
            model = glm::translate(model, center);
            model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
            projection = glm::ortho(-3.5f, 0.5f, -2.f, 2.f, 0.1f, 100.0f);
        } else if (perspective) {
            model = glm::mat4(1.0f);
            projection = glm::mat4(1.0f);
            model = glm::translate(model, center);
            model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
            projection = glm::perspective(glm::radians(-45.0f), (float)width / (float)height, 0.1f, 100.0f);
        } else if (View) {
            model = glm::mat4(1.0f);
            float r = 2.0f;
            float camPosX = sin(glfwGetTime() * 5.f) * r;
            float camPosZ = cos(glfwGetTime() * 5.f) * r;
            view = glm::lookAt(glm::vec3(camPosX, 0.0, camPosZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
            model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
        } else if (bouns_key) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                key_control.moveForward();
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                key_control.moveBack();
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                key_control.moveLeft();
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                key_control.moveRight();
            key_control.rotate(x_pos, y_pos);
            view = glm::lookAt(key_control.cameraPos, key_control.cameraPos + key_control.cameraFront, key_control.cameraUp);
            model = glm::rotate(model, glm::radians(280.f), glm::vec3(0.0f, 1.0f, 0.0f));
            projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
        } else {
            model = glm::mat4(1.0f);
            model = glm::translate(model, center);
            model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
            projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
        }
        

        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc  = glGetUniformLocation(shaderProgram, "view");
        unsigned int project  = glGetUniformLocation(shaderProgram, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(project, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        if (glfwGetKey(window, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_W)) glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
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

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    x_pos = xpos;
    y_pos = ypos;
}



