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
#include <stdlib.h>

using namespace std;

int width = 1000;
int height = 720;

void reset(float& am_factor, float& sp_factor,float& di_factor,int& sh_factor) {
    am_factor = 0.3f;
    sp_factor = 0.1f;
    di_factor = 1.0f;
    sh_factor = 64;
}

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"layout (location = 2) in vec2 aTexCoords;\n"
"out vec3 FragPos;\n"
"out vec3 Normal;\n"
"out vec4 FragPosLightSpace;\n"
"out vec2 TexCoords;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform mat4 lightSpaceMatrix;\n"
"void main()\n"
"{\n"
"       gl_Position = projection * view * model * vec4(aPos,1.0);\n"
"       FragPos = vec3(model * vec4(aPos, 1.0));\n"
"       Normal = transpose(inverse(mat3(model))) * aNormal;\n"
"       TexCoords = aTexCoords;\n"
"       FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);\n"
"}\n\0";

const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec4 FragPosLightSpace;\n"
"in vec3 Normal;\n"
"in vec3 FragPos;\n"
"in vec2 TexCoords;\n"
"uniform sampler2D shadowMap;\n"
"uniform vec3 lightPos;\n"
"uniform vec3 viewPos;\n"
"uniform vec3 lightColor;\n"
"uniform vec3 objectColor;\n"
"uniform float ambient_factor;\n"
"uniform float specular_factor;\n"
"uniform float diffuse_factor;\n"
"uniform float shininess_factor;\n"
"uniform int choose;\n"
"float random(vec3 seed, int i){\n"
"    vec4 seed4 = vec4(seed,i);\n"
"    float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));\n"
"    return fract(sin(dot_product) * 43758.5453);\n"
"}\n"
"float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)\n"
"{\n"
"    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;\n"
"    projCoords = projCoords * 0.5 + 0.5;\n"
"    float closestDepth = texture(shadowMap, projCoords.xy).r;\n"
"    float currentDepth = projCoords.z;\n"
"    float shadow = 0.0;\n"
"    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);\n"
"    if (choose ==  1) {\n"
"       shadow = currentDepth> closestDepth + bias ? 1.0 : 0.0;\n"
"    } else if (choose == 2) {\n"
"       vec2 texelSize = 1.0 / textureSize(shadowMap, 0);\n"
"       for(int x = -1; x <= 1; ++x) { \n"
"           for(int y = -1; y <= 1; ++y) { \n"
"               float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;\n"
"               shadow += currentDepth > pcfDepth + bias ? 1.0 : 0.0;\n"
"           }\n"
"       }\n"
"       shadow /= 9.0;\n"
"   } else if (choose == 3) {\n"
"       vec2 texelSize = 1.0 / textureSize(shadowMap, 0);\n"
"       vec2 poissonDisk[16] = vec2[](\n"
"                              vec2( -0.94201624, -0.39906216 ),\n"
"                              vec2( 0.94558609, -0.76890725 ),\n"
"                              vec2( -0.094184101, -0.92938870 ),\n"
"                              vec2( 0.34495938, 0.29387760 ),\n"
"                              vec2( -0.91588581, 0.45771432 ),\n"
"                              vec2( -0.81544232, -0.87912464 ),\n"
"                              vec2( -0.38277543, 0.27676845 ),\n"
"                              vec2( 0.97484398, 0.75648379 ),\n"
"                              vec2( 0.44323325, -0.97511554 ),\n"
"                              vec2( 0.53742981, -0.47373420 ),\n"
"                              vec2( -0.26496911, -0.41893023 ),\n"
"                              vec2( 0.79197514, 0.19090188 ),\n"
"                              vec2( -0.24188840, 0.99706507 ),\n"
"                              vec2( -0.81409955, 0.91437590 ),\n"
"                              vec2( 0.19984126, 0.78641367 ),\n"
"                              vec2( 0.14383161, -0.14100790 )\n"
"                              );\n"
"           for(int i = 0; i < 6; ++i) { \n"
"               int index = int(16*random(projCoords, i)) % 16;\n"
"               float pcfDepth = texture(shadowMap, projCoords.xy +  poissonDisk[index] * texelSize).r;\n"
"               shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;\n"
"           }\n"
"           shadow /= 6;\n"
"}\n"
"    if(projCoords.z > 1.0)\n"
"    shadow = 0.0;\n"
"    return shadow;\n"
"}\n"
"void main()\n"
"{\n"
"       vec3 ambient = ambient_factor * lightColor;\n"
"       vec3 norm = normalize(Normal);\n"
"       vec3 lightDir = normalize(lightPos - FragPos);\n"
"       float diff = max(dot(norm, lightDir), 0.0);\n"
"       vec3 diffuse = diffuse_factor * diff * lightColor;\n"
"       vec3 viewDir = normalize(viewPos - FragPos);\n"
"       vec3 reflectDir = reflect(-lightDir, norm);\n"
"       float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess_factor);\n"
"       vec3 specular = specular_factor * spec * lightColor;\n"
"       float shadow = ShadowCalculation(FragPosLightSpace, norm, lightDir);\n"
"       vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * objectColor;\n"
"       FragColor = vec4(lighting, 1.0);\n"
"}\n\0";

const char *shadow_mapping_vec = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"uniform mat4 lightSpaceMatrix;\n"
"uniform mat4 model;\n"
"void main()\n"
"{\n"
"gl_Position = lightSpaceMatrix * model * vec4(position, 1.0f);\n"
"}\n";

const char *shadow_mapping_frg = "#version 330 core\n"
"void main()\n"
"{\n"
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
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
        
        1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
        1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
        1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
        1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
        1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
        
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
        1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
        1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
        1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
        
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f,
        1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f
    };
    unsigned int cubeVAO;
    unsigned int cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

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

    int depthShaderVec = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(depthShaderVec, 1, &shadow_mapping_vec, NULL);
    glCompileShader(depthShaderVec);

    int depthShaderFrg = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(depthShaderFrg, 1, &shadow_mapping_frg, NULL);
    glCompileShader(depthShaderFrg);

    int depthProgram = glCreateProgram();
    glAttachShader(depthProgram, depthShaderVec);
    glAttachShader(depthProgram, depthShaderFrg);
    glLinkProgram(depthProgram);
    
    GLfloat planeVertices[] = {
        15.0f, -0.5f,  15.0f,  0.0f, 1.0f, 0.0f,
        -15.0f, -0.5f,  15.0f,  0.0f, 1.0f, 0.0f,
        -15.0f, -0.5f, -15.0f,  0.0f, 1.0f, 0.0f,
        
        15.0f, -0.5f,  15.0f,  0.0f, 1.0f, 0.0f,
        -15.0f, -0.5f, -15.0f,  0.0f, 1.0f, 0.0f,
        15.0f, -0.5f, -15.0f,  0.0f, 1.0f, 0.0f
    };
    
    GLuint planeVBO;
    unsigned int planeVAO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    
    GLint max_texture_num = 1024;

    ImGui::StyleColorsLight();

    //set para
    bool show_window = true;
    static float am_factor = 0.3f;
    static float sp_factor = 0.0f;
    static float di_factor = 1.0f;
    static int sh_factor = 64;
    int choose = 1;
    int choose_mapping = 1;
    unsigned int depthMapFBO = 0;
    unsigned int depthMap = 0;
    unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    GLfloat near_plane = 1.0f, far_plane = 7.5f;
    glm::vec3 lightPos;
    lightPos = glm::vec3(1.0f, 4.0f, -2.0f);

    glUseProgram(shaderProgram);
    unsigned int sd_map = glGetUniformLocation(shaderProgram, "shadowMap");
    glUniform1i(sd_map, 1);


    // render loop
    while (!glfwWindowShouldClose(window)) {
        ImGui_ImplGlfwGL3_NewFrame();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        unsigned int choose_type = glGetUniformLocation(shaderProgram, "choose");
        glUniform1i(choose_type, choose);
        if (depthMap == 0 && depthMapFBO == 0) {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            SHADOW_WIDTH = max_texture_num, SHADOW_HEIGHT = max_texture_num;
            glGenFramebuffers(1, &depthMapFBO);
            
            glGenTextures(1, &depthMap);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        if (show_window) {
            ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2(550, 50), ImGuiSetCond_Once);
            ImGui::Begin("Color Tool", &show_window, ImGuiWindowFlags_MenuBar);
            ImGui::Text("Hello");
            ImGui::Text("Choose the different factors");
            ImGui::SliderFloat("ambient factor", &am_factor, 0.1f, 1.5f);
            ImGui::SliderFloat("specular factor", &sp_factor, 0.1f, 100.0f);
            ImGui::SliderFloat("diffuse factor", &di_factor, 0.1f, 3.0f);
            ImGui::SliderInt("shininess factor", &sh_factor, 1, 300);
            ImGui::Text("This is a reset button");
            if (ImGui::Button("Reset_Button")) {
                max_texture_num = 1024;
                depthMapFBO = depthMap = 0;
                reset(am_factor, sp_factor, di_factor, sh_factor);
                choose = 1;
                choose_mapping = 1;
            }
            if (ImGui::CollapsingHeader("Dif Mapping")) {
                if (ImGui::Button("orthographic")) {
                    reset(am_factor, sp_factor, di_factor, sh_factor);
                    choose = 1;
                    choose_mapping = 1;
                }
                if (ImGui::Button("perspective")) {
                    reset(am_factor, sp_factor, di_factor, sh_factor);
                    choose = 1;
                    choose_mapping = 0;
                }
            }
            if (ImGui::CollapsingHeader("Dif Optimization")) {
                if (ImGui::Button("Normal")) {
                    reset(am_factor, sp_factor, di_factor, sh_factor);
                    choose = 1;
                    choose_mapping = 1;
                }
                if (ImGui::Button("PCF")) {
                    reset(am_factor, sp_factor, di_factor, sh_factor);
                    choose = 2;
                    choose_mapping = 1;
                }
                if (ImGui::Button("Poisson")) {
                    reset(am_factor, sp_factor, di_factor, sh_factor);
                    choose = 3;
                    choose_mapping = 1;
                }
                if (ImGui::Button("Texture_size")) {
                    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_num);
                    max_texture_num /= 2;
                    depthMapFBO = depthMap = 0;
                    reset(am_factor, sp_factor, di_factor, sh_factor);
                    choose = 1;
                    choose_mapping = 1;
                }
            }
            ImGui::End();
        }

        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
        glm::vec3 objectColor, lightColor, viewPos;
        float near_plane = 1.0f, far_plane = 7.5f;
        if (choose_mapping)
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        else
        lightProjection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        glUseProgram(0);
        glUseProgram(depthProgram);
        unsigned int LightSpace = glGetUniformLocation(depthProgram, "lightSpaceMatrix");
        glUniformMatrix4fv(LightSpace, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        unsigned int modelLoc_plane1 = glGetUniformLocation(depthProgram, "model");
        model = glm::mat4();
        glUniformMatrix4fv(modelLoc_plane1, 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        unsigned int modelLoc1 = glGetUniformLocation(depthProgram, "model");
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
        model = glm::scale(model, glm::vec3(0.5f));
        glUniformMatrix4fv(modelLoc1, 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, width*2, height*2);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(0);
        glUseProgram(shaderProgram);

        lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        viewPos = glm::vec3(0.0f, 0.0f, 3.0f);
        view = glm::lookAt(glm::vec3(4.0, 4.0, 4.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

        unsigned int LightSpace1 = glGetUniformLocation(shaderProgram, "lightSpaceMatrix");
        glUniformMatrix4fv(LightSpace1, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        unsigned int ambient_part = glGetUniformLocation(shaderProgram, "ambient_factor");
        unsigned int specular_part = glGetUniformLocation(shaderProgram, "specular_factor");
        unsigned int diffuse_part = glGetUniformLocation(shaderProgram, "diffuse_factor");
        unsigned int shininess_part = glGetUniformLocation(shaderProgram, "shininess_factor");

        glUniform1f(ambient_part, am_factor);
        glUniform1f(specular_part, sp_factor);
        glUniform1f(diffuse_part, di_factor);
        glUniform1f(shininess_part, sh_factor);

        unsigned int lightC  = glGetUniformLocation(shaderProgram, "lightColor");
        glUniform3fv(lightC, 1, glm::value_ptr(lightColor));
        unsigned int lightP  = glGetUniformLocation(shaderProgram, "lightPos");
        glUniform3fv(lightP, 1, glm::value_ptr(lightPos));
        unsigned int viewP = glGetUniformLocation(shaderProgram, "viewPos");
        glUniform3fv(viewP, 1, glm::value_ptr(viewPos));

        unsigned int viewLoc  = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        unsigned int project  = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(project, 1, GL_FALSE, glm::value_ptr(projection));


        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glUniform1i(sd_map, 1);


        
        model = glm::mat4();
        objectColor = glm::vec3(1.0, 1.0, 1.0);
        unsigned int modelLoc_plane = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc_plane, 1, GL_FALSE, glm::value_ptr(model));
        unsigned int object_plane = glGetUniformLocation(shaderProgram, "objectColor");
        glUniform3fv(object_plane, 1, glm::value_ptr(objectColor));
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
        model = glm::scale(model, glm::vec3(0.5f));
        objectColor = glm::vec3(0.902f, 0.0f, 0.0f);
        unsigned int object = glGetUniformLocation(shaderProgram, "objectColor");
        glUniform3fv(object, 1, glm::value_ptr(objectColor));
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);


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





