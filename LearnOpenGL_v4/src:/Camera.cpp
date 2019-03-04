//
//  Camera.cpp
//  OpenGL1
//
//  Created by pro on 2018/4/4.
//  Copyright © 2018年 pro. All rights reserved.
//

#include <stdio.h>
#include "Camera.hpp"

Camera::Camera(glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, float cameraSpeed) {
    this -> cameraPos = cameraPos;
    this -> cameraFront = cameraFront;
    this -> cameraUp = cameraUp;
    this -> cameraSpeed = cameraSpeed;
}

//Camera::Camera()

void Camera::moveForward() {
    cameraPos += cameraSpeed * cameraFront;
}

void Camera::moveBack() {
    cameraPos -= cameraSpeed * cameraFront;
}

void Camera::moveLeft() {
    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void Camera::moveRight() {
    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}


void Camera::rotate(float xpos, float ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float x_off = xpos - lastX;
    float y_off = lastY - ypos;
    
    lastX = xpos;
    lastY = ypos;
    
    x_off *= cameraSpeed;
    y_off *= cameraSpeed;
    
    yaw += x_off;
    pitch += y_off;
    
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
    
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}
