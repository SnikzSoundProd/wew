#include "Camera.h"
#include "../core/GameObject.h"
#include <glm/gtc/quaternion.hpp> // Для lerp
#include <GLFW/glfw3.h> // <-- ДЛЯ ФУНКЦИЙ GLFW
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp> 

Camera::Camera() {
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() {
    if (target) {
        glm::vec3 targetPos = target->transform->position + glm::vec3(0.0f, 1.0f, 0.0f);
        return glm::lookAt(position, targetPos, worldUp);
    }
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix(float width, float height) {
    return glm::perspective(glm::radians(fov), width / height, 0.1f, 100.0f);
}

void Camera::processKeyboard(const char* direction, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    if (strcmp(direction, "FORWARD") == 0) position += front * velocity;
    if (strcmp(direction, "BACKWARD") == 0) position -= front * velocity;
    if (strcmp(direction, "LEFT") == 0) position -= right * velocity;
    if (strcmp(direction, "RIGHT") == 0) position += right * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    // В игровом режиме вращаем камеру вокруг цели
    if (target) {
        angleAroundTarget -= xoffset * mouseSensitivity * 2.0f;
        pitchOffset -= yoffset * mouseSensitivity * 2.0f;
        if (pitchOffset > 89.0f) pitchOffset = 89.0f;
        if (pitchOffset < 5.0f) pitchOffset = 5.0f;
    } 
    // В режиме редактора - свободный полет
    else {
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;
        yaw += xoffset;
        pitch += yoffset;
        if (constrainPitch) {
            if (pitch > 89.0f) pitch = 89.0f;
            if (pitch < -89.0f) pitch = -89.0f;
        }
        updateCameraVectors();
    }
}

void Camera::followTarget(float deltaTime) {
    if (!target) return;

    float horizontalDist = distanceToTarget * cos(glm::radians(pitchOffset));
    float verticalDist = distanceToTarget * sin(glm::radians(pitchOffset));
    float theta = glm::radians(angleAroundTarget);
    float offsetX = horizontalDist * sin(theta);
    float offsetZ = horizontalDist * cos(theta);

    glm::vec3 targetPos = target->transform->position;
    glm::vec3 desiredPosition = glm::vec3(targetPos.x - offsetX, targetPos.y + verticalDist, targetPos.z - offsetZ);

    position = glm::lerp(position, desiredPosition, followSpeed * deltaTime);
}

void Camera::updateCameraVectors() {
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}