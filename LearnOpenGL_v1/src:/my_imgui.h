//
//  imgui_impl_glfw_gl3.h
//  OpenGL1
//
//  Created by pro on 2018/3/14.
//  Copyright © 2018年 pro. All rights reserved.
//

struct GLFWwindow;

void my_imgio_Init(GLFWwindow* window);
void my_imgio_NewFrame();
void my_imgio_RenderData(ImDrawData* draw);
bool my_imgio_CreateTexture();
bool my_imgio_GreateObjs();
