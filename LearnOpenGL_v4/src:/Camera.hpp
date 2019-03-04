//
//  Camera.hpp
//  OpenGL1
//
//  Created by pro on 2018/4/3.
//  Copyright © 2018年 pro. All rights reserved.
//

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdio.h>

class Camera {
public:
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    float cameraSpeed;
    
    Camera(glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, float cameraSpeed);
    void moveForward();
    void moveBack();
    void moveRight();
    void moveLeft();
    void rotate(float xpos, float ypos);
private:
    float lastX = 1000.0f / 2.f;
    float lastY = 720.0f / 2.f;
    float yaw   = -90.0f;
    float pitch =  0.0f;
    bool firstMouse = true;
};

