#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <vector>

using namespace std;

int width = 1000;
int height = 720;

typedef struct point { // point struct
    int x;
    int y;
    point(int x1, int y1) {
        x = x1;
        y = y1;
    }
}my_point;

void set_adder(int &dx_y, int &adder) {
    if (dx_y < 0) {
        dx_y = abs(dx_y);
        adder = -1;
    } else {
        adder = 1;
    }
}

bool Boundry(int x, int y) { // judge boundry
    if (x < (-width / 2) || x > width / 2) return false;
    if (y < (-width / 2) || y > width / 2) return false;
    return true;
}

void Draw_Point (GLint x, GLint y, bool flag) { // draw point
    float temp_x = x / (float)width;
    float temp_y = y / (float)height;
    float color_r, color_g;
    if (flag == true) {
        color_r = 1.0f;
        color_g = 0.0f;
    } else {
        color_r = 0.0f;
        color_g = 1.0f;
    }
    float vertices_line[] = {
        temp_x, temp_y, 1.0f, color_r, color_g, 0.0f
    };
    
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_line), vertices_line, GL_STATIC_DRAW);
    glDrawArrays(GL_POINTS, 0, 1);
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

bool Line_Bresenham (int x0, int y0, int x1, int y1) {
    if (Boundry(x0, y0) && Boundry(x1, y1)) {
        int dx, dy;
        int add_x, add_y;

        dx = x1 - x0;
        dy = y1 - y0;

        set_adder(dy, add_y);
        set_adder(dx, add_x);
        int dx_2 = dx * 2;
        int dy_2 = dy * 2;

        Draw_Point(x0, y0, true);

        if (dx > dy) {
            int p_state = dy_2 - (dx);
            while (x0 != x1) {
                x0 += add_x;
                if (p_state >= 0) {
                    y0 += add_y;
                    p_state = p_state - dx_2 + dy_2;
                }
                p_state += dy_2;
                Draw_Point(x0, y0, true);
            }
            } else {
                int p_state = dx_2 - dy;
                while (y0 != y1) {
                    y0 += add_y;
                    if (p_state >= 0) {
                        x0 += add_x;
                        p_state = p_state - dy_2 + dx_2;
                    }
                    p_state += dx_2;
                    Draw_Point(x0, y0, true);
                }
            }
        return true;
        }
    return false;
}

bool Circle_Bresenham(int center_x, int center_y, int r) {
    if (!Boundry(center_x, center_y)) return false;
    Draw_Point(center_x, center_y, true);
    float p0 = 5/(float)4 - r;
    float x0 = 0, y0 = r;
    for (; x0 <= y0; x0++) {
        if (p0 < 0) {
            Draw_Point(x0 + 1 + center_x, y0 + center_y, true);
            Draw_Point(- (x0 + 1) + center_x, y0 + center_y, true);
            Draw_Point(x0 + 1 + center_x, -y0 + center_y, true);
            Draw_Point(- (x0 + 1) + center_x, -y0 + center_y, true);

            Draw_Point(y0 + center_x, x0 + 1 + center_y, true);
            Draw_Point(-y0 + center_x, x0 + 1 + center_y, true);
            Draw_Point(-y0 + center_x, -(x0 + 1) + center_y, true);
            Draw_Point(y0 + center_x, -(x0 + 1) + center_y, true);
            p0 = p0 + 2 * x0 + 3;
        } else {
            Draw_Point(x0 + 1 + center_x, y0 - 1 + center_y, true);
            Draw_Point(- (x0 + 1) + center_x, y0 - 1 + center_y, true);
            Draw_Point(x0 + 1 + center_x, -(y0 - 1) + center_y, true);
            Draw_Point(-(x0 + 1) + center_x, -(y0 - 1) + center_y, true);

            Draw_Point(y0 - 1 + center_x, x0 + 1 + center_y, true);
            Draw_Point(y0 - 1 + center_x, -(x0 + 1) + center_y, true);
            Draw_Point(-(y0 - 1) + center_x, -(x0 + 1) + center_y, true);
            Draw_Point(-(y0 - 1) + center_x, x0 + 1 + center_y, true);
            p0 = p0 + 2 * x0 - 2 * y0 + 5;
            y0--;
        }
    }
    return true;
}

int max_num(int x, int y, int z) {
    return max(max(x, y), z);
}

int min_num(int x, int y, int z) {
    return min(min(x, y), z);
}

void Raster(point p1, point p2, point p3) {
    int x_min = min(min(p1.x, p2.x), p3.x);
    int x_max = max(max(p1.x, p2.x), p3.x);
    int y_min = min(min(p1.y, p2.y), p3.y);
    int y_max = max(max(p1.y, p2.y), p3.y);
    float signOfTrig = (p2.x - p1.x)*(p3.y - p1.y) - (p2.y - p1.y)*(p3.x - p1.x);
    for (int i = y_min; i < y_max; i++) {
        for (int j = x_min; j < x_max; j++) {
            float signOfAB = (p2.x - p1.x) * (i - p1.y) - (p2.y - p1.y) * (j - p1.x);
            float signOfCA = (p1.x - p3.x) * (i - p3.y) - (p1.y - p3.y) * (j - p3.x);
            float signOfBC = (p3.x - p2.x) * (i - p2.y) - (p3.y - p2.y) * (j - p2.x);
            bool d1 = (signOfAB * signOfTrig > 0);
            bool d2 = (signOfCA * signOfTrig > 0);
            bool d3 = (signOfBC * signOfTrig > 0);
            if (d1 && d2 && d3) {
                Draw_Point(j, i, false);
            }
        }
    }
}

bool in_or_not(float a, float b, float c, float d, float x, float y) {
    return (b-d) * x + (c - a) * y + a * d - c * d;
}

const char *vertexShaderSource = "#version 330 core\n"
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
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, 1.0f);\n"
"}\n\0";

int main(int, char**) {
    // defination of the point we use
    point p1(0 , 0), p2(200, -200), p3(-200, -200);
    point center(100, -100);
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
    bool is_Bresenham = false;
    bool is_Bresenham_Circle = false;
    bool Rasterization_it = false;
    static int i1 = 100;
    glPointSize(3.0);
    // render loop
    while (!glfwWindowShouldClose(window)) {
        ImGui_ImplGlfwGL3_NewFrame();

        if (show_window) {
            ImGui::SetNextWindowSize(ImVec2(400, 220), ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiSetCond_Once);
            ImGui::Begin("Color Tool", &show_window);
            ImGui::Text("Hello");
            ImGui::Text("This is basic part 2.....((/- -)/");
            if (ImGui::Button("Bresenham Circle")) {
                // judge
                is_Bresenham = false;
                is_Bresenham_Circle = true;
                Rasterization_it = false;
            }
            ImGui::Text("Choose a radius of the circle .....((/- -)/");
            ImGui::SliderInt("slider int", &i1, 1, 300);
            ImGui::Text("This is basic part 1.....((/- -)/");
            if (ImGui::Button("Bresenham Traingle")) {
                // judge
                is_Bresenham = true;
                is_Bresenham_Circle = false;
                Rasterization_it = false;
            }
            ImGui::Text("It will take a long time.....((/- -)/");
            if (ImGui::Button("Rasterization")) {
                // judge
                Rasterization_it = true;
            }
            ImGui::End();
        }

        if (is_Bresenham) {
            Line_Bresenham(p1.x, p1.y, p2.x, p2.y);
            Line_Bresenham(p1.x, p1.y, p3.x, p3.y);
            Line_Bresenham(p2.x, p2.y, p3.x, p3.y);
            if (Rasterization_it) Raster(p1, p2, p3);
        } else if (is_Bresenham_Circle) {
            Circle_Bresenham(center.x, center.y, i1);
        }
        
        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
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






