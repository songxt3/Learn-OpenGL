#include "imgui.h"
#include "my_imgui.h"
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void set_paras(bool &para1, bool &para2, bool &para3) {
    para1 = true;
    para2 = false;
    para3 = false;
}
// traingle 1
const char *vertexShaderSource ="#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"   ourColor = aColor;\n"
"}\0";

const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"uniform vec4 changeColor;\n"
"void main()\n"
"{\n"
"if (changeColor == vec4(0.6f, 0.6f, 0.6f, 1.00f))\n"
"   FragColor = vec4(ourColor, 1.0f);\n"
"else\n"
"   FragColor = changeColor;\n"
"}\n\0";

// mul traingles
const char *vertexShaderSource1 = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"}\0";

const char *fragmentShaderSource1 = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.0f, 0.2f, 0.5f, 1.0f);\n"
"}\n\0";

int main(int, char**) {
    // initialize
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1000, 720, "CG hw1", NULL, NULL);
    glfwMakeContextCurrent(window);

    //initial glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
    }

    // Setup ImGui binding
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    my_imgio_Init(window);

    // inital vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);


    // initial fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);


    // link config
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // delete shader
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {      // position and color
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f

    };

    // inital VAO and VBO
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    int vertexShader1 = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader1, 1, &vertexShaderSource1, NULL);
    glCompileShader(vertexShader1);

    int fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader1, 1, &fragmentShaderSource1, NULL);
    glCompileShader(fragmentShader1);

    int shaderProgram1 = glCreateProgram();
    glAttachShader(shaderProgram1, vertexShader1);
    glAttachShader(shaderProgram1, fragmentShader1);
    glLinkProgram(shaderProgram1);

    glDeleteShader(vertexShader1);
    glDeleteShader(fragmentShader1);
    
    // this is mul
    float vertices_mul[] = {
        0.2f, 0.0f, 0.0f,
        0.4f, 0.0f, 0.0f,
        0.3f, 0.3f, 0.0f,
    
        -0.2f, -0.5f, 0.0f,
        0.2f, -0.5f, 0.0f,
        0.0f, -0.6f, 0.0f,
    
        -0.4f, 0.0f, 0.0f,
        -0.2f, 0.0f, 0.0f,
        -0.3f, 0.3f, 0.0f
    };
    unsigned int indices[] = {
        0, 1, 2,
        3, 4, 5,
        6, 7, 8
    };
    unsigned int VBO1, VAO1, EBO;
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO1);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_mul), vertices_mul, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    //this is point
    float vertices_line[] = {
        -0.5f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f,
        0.5f, 0.0f, 0.0f
    };
    
    unsigned int VBO_line, VAO_line;
    glGenVertexArrays(1, &VAO_line);
    glGenBuffers(1, &VBO_line);
    
    glBindVertexArray(VAO_line);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_line), vertices_line, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindVertexArray(0);
    
    ImGui::StyleColorsLight();

    // window flag
    bool show_window = true;
    bool another_window = true;
    bool is_triangle = false;
    bool is_mul_triangles = false;
    bool is_line = false;
    ImVec4 change_color = ImVec4(0.6f, 0.6f, 0.6f, 1.00f);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        my_imgio_NewFrame();

        // window body
        if (show_window) {
            ImGui::SetNextWindowSize(ImVec2(400, 100), ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2(550, 50), ImGuiSetCond_Once);
            ImGui::Begin("Color Tool", &show_window);
            ImGui::Text("Hello");
            ImGui::Text("Choose a color you like .....((/- -)/");
            ImGui::ColorEdit3("clear color", (float*)&change_color);
            int vertexColorLocation = glGetUniformLocation(shaderProgram, "changeColor");
            glUniform4f(vertexColorLocation, change_color.x, change_color.y, change_color.z, change_color.w);
            ImGui::End();
        }
        // another window
        if (another_window) {
            ImGui::SetNextWindowSize(ImVec2(400, 100), ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiSetCond_Once);
            ImGui::Begin("Choose moudle", &another_window);
            ImGui::Text("Hello");
            ImGui::Text("Choose a moudle you like .....((/- -)/");
            if (ImGui::Button("Triangle")) {
                // judge
                set_paras(is_triangle, is_mul_triangles, is_line);
            }
            ImGui::SameLine();
            if (ImGui::Button("Three Triangles")) {
                set_paras(is_mul_triangles, is_triangle, is_line);
            }
            ImGui::SameLine();
            if (ImGui::Button("Line")) {
                set_paras(is_line, is_mul_triangles, is_triangle);
            }
            ImGui::End();
        }

        glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // render ImGui
        ImGui::Render();
        my_imgio_RenderData(ImGui::GetDrawData());
        if (is_line) {
            glBindVertexArray(VAO_line);
            glUseProgram(shaderProgram1);
            glDrawArrays(GL_LINE_STRIP, 0, 3);
        } else if (is_mul_triangles) {
            glUseProgram(shaderProgram1);
            glBindVertexArray(VAO1);
            glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
        } else {
            glBindVertexArray(VAO);
            glUseProgram(shaderProgram);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(1, &VBO1);
    ImGui::DestroyContext();
    glfwTerminate();
    

    return 0;
}


