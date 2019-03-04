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
        -0.2f, -0.2f, -0.2f, 1.0f, 0.0f, 0.0f,
        0.2f, -0.2f, -0.2f, 0.0f, 1.0f, 0.0f,
        0.2f,  0.2f, -0.2f, 1.0f, 0.0f, 0.0f,
        0.2f,  0.2f, -0.2f, 1.0f, 0.0f, 0.0f,
        -0.2f,  0.2f, -0.2f, 0.0f, 0.0f, 1.0f,
        -0.2f, -0.2f, -0.2f, 1.0f, 0.0f, 0.0f,

        -0.2f, -0.2f,  0.2f, 0.0f, 1.0f, 0.0f,
        0.2f, -0.2f,  0.2f, 0.0f, 0.0f, 1.0f,
        0.2f,  0.2f,  0.2f, 1.0f, 0.0f, 0.0f,
        0.2f,  0.2f,  0.2f, 1.0f, 0.0f, 0.0f,
        -0.2f,  0.2f,  0.2f, 0.0f, 0.0f, 1.0f,
        -0.2f, -0.2f,  0.2f, 0.0f, 1.0f, 0.0f,

        -0.2f,  0.2f,  0.2f, 0.0f, 0.0f, 1.0f,
        -0.2f,  0.2f, -0.2f, 1.0f, 0.0f, 0.0f,
        -0.2f, -0.2f, -0.2f, 0.0f, 1.0f, 0.0f,
        -0.2f, -0.2f, -0.2f, 0.0f, 1.0f, 0.0f,
        -0.2f, -0.2f,  0.2f, 1.0f, 0.0f, 0.0f,
        -0.2f,  0.2f,  0.2f, 0.0f, 0.0f, 1.0f,

        0.2f,  0.2f,  0.2f, 0.0f, 0.0f, 1.0f,
        0.2f,  0.2f, -0.2f, 0.0f, 1.0f, 0.0f,
        0.2f, -0.2f, -0.2f, 1.0f, 0.0f, 0.0f,
        0.2f, -0.2f, -0.2f, 1.0f, 0.0f, 0.0f,
        0.2f, -0.2f,  0.2f, 0.0f, 1.0f, 0.0f,
        0.2f,  0.2f,  0.2f, 0.0f, 0.0f, 1.0f,

        -0.2f, -0.2f, -0.2f, 1.0f, 0.0f, 0.0f,
        0.2f, -0.2f, -0.2f, 0.0f, 1.0f, 0.0f,
        0.2f, -0.2f,  0.2f, 0.0f, 0.0f, 1.0f,
        0.2f, -0.2f,  0.2f, 0.0f, 0.0f, 1.0f,
        -0.2f, -0.2f,  0.2f, 0.0f, 1.0f, 0.0f,
        -0.2f, -0.2f, -0.2f, 1.0f, 0.0f, 0.0f,

        -0.2f,  0.2f, -0.2f, 0.0f, 0.0f, 1.0f,
        0.2f,  0.2f, -0.2f, 1.0f, 0.0f, 0.0f,
        0.2f,  0.2f,  0.2f, 0.0f, 1.0f, 0.0f,
        0.2f,  0.2f,  0.2f,  0.0f, 1.0f, 0.0f,
        -0.2f,  0.2f,  0.2f,  1.0f, 0.0f, 0.0f,
        -0.2f,  0.2f, -0.2f, 0.0f, 0.0f, 1.0f
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
    bool Translation = false;
    bool Rotation = false;
    bool show_window = true;
    bool Scaling = false;
    bool bonus = false;
    float trans_dis = 0.0f;
    
    // render loop
    while (!glfwWindowShouldClose(window)) {
        ImGui_ImplGlfwGL3_NewFrame();
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        if (show_window) {
            ImGui::SetNextWindowSize(ImVec2(400, 250), ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiSetCond_Once);
            ImGui::Begin("Color Tool", &show_window, ImGuiWindowFlags_MenuBar);
            ImGui::Text("Hello");
            ImGui::Text("Choose the operation");
            if (ImGui::CollapsingHeader("Translation")) {
                if (ImGui::Button("Start Translating")) {
                    Translation = true;
                    Rotation = Scaling = false;
                }
            }
            if (ImGui::CollapsingHeader("Rotation")) {
                if (ImGui::Button("Start Rotating")) {
                    Translation = Scaling = false;
                    Rotation = true;
                }
            }
            if (ImGui::CollapsingHeader("Scaling")) {
                if (ImGui::Button("Start Scaling")) {
                    Translation = Rotation = false;
                    Scaling = true;
                }
            }
            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("Reset")) {
                    Translation = Rotation = Scaling = false;
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            ImGui::End();
        }

        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;

        glUseProgram(shaderProgram);

        if (Translation) {
            float trans_dis = sin((abs(glfwGetTime())));
            model = glm::mat4();
            model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::translate(model, glm::vec3(0.0f, trans_dis, 0.0f));
            view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
            projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
        } else if (Rotation) {
            model = glm::mat4();
            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 1.0f));
            view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
            projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
        } else if (Scaling) {
            model = glm::mat4();
            float max_time = sin((abs(glfwGetTime()))) + 1.0f;
            model = glm::scale(model, glm::vec3(max_time, max_time, max_time));
            model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 1.0f));
            view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
            projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
        } else {
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

