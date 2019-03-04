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
#include <vector>

#define POINTNUM 4

using namespace std;

int width = 1000;
int height = 720;
bool judge_mouse = false;
int display_w = width, display_h = height;

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

typedef struct point { // point struct
    float x;
    float y;
    point(float x1, float y1) {
        x = x1;
        y = y1;
    }
}my_point;

void Draw_Point (GLint x, GLint y, GLfloat point_size) { // draw point
    float temp_x = x / (float)display_w;
    float temp_y = y / (float)display_h;
    float color_r, color_g;
    color_r = 0.0f;
    color_g = 1.0f;
    float vertices_line[] = {
        temp_x, temp_y, 1.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_line), vertices_line, GL_STATIC_DRAW);
    glPointSize(point_size);
    glDrawArrays(GL_POINTS, 0, 1);
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void Draw_Line (vector<float> my_line) {
    float vertices_line[my_line.size()];
        for (int i = 0; i < my_line.size(); i++) {
            vertices_line[i] = my_line[i];
    }
    

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_line), vertices_line, GL_STATIC_DRAW);
    glDrawArrays(GL_LINES, 0, 2);
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

vector<float> drawBezier(point p1, point p2, point p3, point p4,double t) {
    float x = 0;
    float y = 0;
    float a1 = pow((1-t),3);
    float a2 = pow((1-t),2)*3*t;
    float a3 = 3*t*t*(1-t);
    float a4 = t*t*t;
    vector<float> my_line;
    
    x = a1*p1.x + a2*p2.x + a3*p3.x + a4*p4.x;
    y = a1*p1.y + a2*p2.y + a3*p3.y + a4*p4.y;
    
    my_line.push_back(x / (float)display_w);
    my_line.push_back(y / (float)display_h);
    my_line.push_back(1.0f);
    
    return my_line;
}

vector<int> x_set;
vector<int> y_set;
bool change_pos = false;
bool draw = false;
int flag = 0;


const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0f);\n"
"}\0";

const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"uniform vec4 changeColor;\n"
"void main()\n"
"{\n"
"   FragColor = changeColor;\n"
"}\n\0";

int main(int, char**) {
    // defination of the point we use
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

    glUseProgram(shaderProgram);

    ImGui::StyleColorsLight();

    // window flag
    bool show_window = true;
    bool Bezier_Curve = false;
    int state = GLFW_RELEASE;
    vector<float> temp;
    vector<float> res;
    bool initial = true;
    glm::vec4 color;
    color = glm::vec4(0.902f, 0.0f, 0.0f, 1.0f);
    temp.clear();
    double x_pos = 0, y_pos = 0;
    double x_last = 0, y_last = 0;
    int count = 0;
    static int x_point = 0;
    static int y_point = 0;
    bool judge_det = true;
    ImVec2 win_pos;
    ImVec2 win_size;
    x_set.clear();
    y_set.clear();
    // render loop
    while (!glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        display_w = display_w / 2;
        display_h = display_h / 2;
        ImGui_ImplGlfwGL3_NewFrame();
        unsigned int choose_color = glGetUniformLocation(shaderProgram, "changeColor");
        glUniform4fv(choose_color, 1, glm::value_ptr(color));
        if (show_window) {
            ImGui::SetNextWindowSize(ImVec2(320, 250), ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiSetCond_Once);
            ImGui::Begin("Color Tool", &show_window);
            ImGui::Text("Draw points must be four~~~");
            ImGui::Text("Hello");
            if (ImGui::Button("Bezier Curve")) {
                // judge
                Bezier_Curve = true;
            }
            ImGui::ColorEdit3("change color", (float*)&color);
            x_point = 0, y_point = 0;
            if (!x_set.empty()) {
            x_point = int(x_set[0]);
            y_point = int(y_set[0]);
            }
            ImGui::Text("point 1 position (%d,%d)", x_point, y_point);
            ImGui::SameLine();
            if (ImGui::Button("DELETE POINT1")) {
                // judge
                if (!x_set.empty()) {
                    count--;
                    x_set.erase(x_set.begin());
                    y_set.erase(y_set.begin());
                }
            }
            x_point = 0, y_point = 0;
            if (x_set.size() >= 2) {
                x_point = int(x_set[1]);
                y_point = int(y_set[1]);
            }
            ImGui::Text("point 2 position (%d,%d)", x_point, y_point);
            ImGui::SameLine();
            if (ImGui::Button("DELETE POINT2")) {
                // judge
                if (x_set.size() >= 2) {
                    count--;
                    x_set.erase(x_set.begin() + 1);
                    y_set.erase(y_set.begin() + 1);
                }
            }
            x_point = 0, y_point = 0;
            if (x_set.size() >= 3) {
                x_point = int(x_set[2]);
                y_point = int(y_set[2]);
            }
            ImGui::Text("point 3 position (%d,%d)", x_point, y_point);
            ImGui::SameLine();
            if (ImGui::Button("DELETE POINT3")) {
                // judge
                if (x_set.size() >= 3) {
                    count--;
                    x_set.erase(x_set.begin() + 2);
                    y_set.erase(y_set.begin() + 2);
                }
            }
            x_point = 0, y_point = 0;
            if (x_set.size() >= 4) {
                x_point = int(x_set[3]);
                y_point = int(y_set[3]);
            }
            ImGui::Text("point 4 position (%d,%d)", x_point, y_point);
            ImGui::SameLine();
            if (ImGui::Button("DELETE POINT4")) {
                // judge
                if (x_set.size() >= 4) {
                    count--;
                    x_set.erase(x_set.begin() + 3);
                    y_set.erase(y_set.begin() + 3);
                }
            }
            if (ImGui::Button("DELETE ALL")) {
                // judge
                if (x_set.size() >= 4) {
                    count = 0;
                    x_set.clear();
                    y_set.clear();
                    Bezier_Curve = false;
                }
            }
            win_pos = ImGui::GetWindowPos();
            win_size = ImGui::GetWindowSize();
            ImGui::End();
        }
        glfwGetCursorPos(window, &x_pos, &y_pos);
        if ((x_pos < win_pos.x + win_size.x && x_pos > win_pos.x) && (y_pos < win_pos.y + win_size.y && y_pos > win_pos.y)) {
            judge_det = false;
        } else {
            judge_det = true;
        }
        if (state != GLFW_RELEASE && judge_det) {
            glfwGetCursorPos(window, &x_pos, &y_pos);
            if (x_pos != x_last && y_pos != y_last && count < 4) {
                count++;
                x_set.push_back(2 * x_pos - display_w);
                y_set.push_back(-(2 * y_pos - display_h));
                }
                x_last = x_pos;
                y_last = y_pos;
            }
            state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (count == 4) {
            glfwGetCursorPos(window, &x_pos, &y_pos);
            for (int i = 0; i < POINTNUM; i++) {
                if (((int(2 * x_pos - display_w) > int(x_set[i]) - 5) && (int(2 * x_pos - display_w) < int(x_set[i]) + 5)) && ((int(-(2 * y_pos - display_h)) > int(y_set[i]) - 5) && (int(-(2 * y_pos - display_h)) < int(y_set[i]) + 5)) ) {
                    ImGui::SetMouseCursor(3);
                }
            }
        }
        for (int i = 0 ; i < x_set.size(); i++) {
            Draw_Point(x_set[i], y_set[i], 8.0f);
        }
        if (Bezier_Curve) {
            point p1(x_set[0], y_set[0]), p2(x_set[1], y_set[1]), p3(x_set[2], y_set[2]), p4(x_set[3], y_set[3]);
            if (initial) {
                temp.push_back(p1.x / (float)display_w);
                temp.push_back(p1.y / (float)display_h);
                temp.push_back(1.0f);
            }
            for (double i = 0.0f; i <= 1.05f; i+=0.05f) {
                initial = false;
                res.clear();
                vector<float> my_line = drawBezier(p1, p2, p3, p4, i);
                res.insert(res.end(),temp.begin(),temp.end());
                res.insert(res.end(),my_line.begin(),my_line.end());
                if (i != 0.0f) Draw_Line(res);
                temp = my_line;
            }
        }
        
//        Draw_Line();

        // Rendering
//        glfwGetFramebufferSize(window, &display_w, &display_h);
//        glViewport(0, 0, display_w, display_h);
        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwPollEvents();
    }

    // cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (!x_set.empty() && x_set.size() == 4) {
//        Draw_Point(2 * xpos - display_w, 2 * -ypos + display_h, 20.0f);
        for (int i = 0; i < POINTNUM; i++) {
            if (((int(2 * xpos - display_w) > int(x_set[i]) - 5) && (int(2 * xpos - display_w) < int(x_set[i]) + 5)) && ((int(-(2 * ypos - display_h)) > int(y_set[i]) - 5) && (int(-(2 * ypos - display_h)) < int(y_set[i]) + 5)) ) {
                if (state == GLFW_PRESS) {
                    change_pos = true;
                    flag = i;
                }
            }
        }
        if (state == GLFW_RELEASE) change_pos = false;
        if (change_pos) {
            x_set[flag] = 2 * xpos - display_w;
            y_set[flag] = -(2 * ypos - display_h);
        }
    }
}


