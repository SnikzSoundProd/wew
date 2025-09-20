#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class GameObject; // Forward declaration

class Camera {
public:
    // Поля для свободного полета
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    float yaw = -90.0f;
    float pitch = 0.0f;
    float movementSpeed = 2.5f;
    float mouseSensitivity = 0.1f;
    float fov = 45.0f;

    // Поля для режима следования
    GameObject* target = nullptr;
    float distanceToTarget = 6.0f;
    float angleAroundTarget = 0.0f;
    float pitchOffset = 30.0f;
    float followSpeed = 5.0f;
    bool invertX = true;

    // --- ОБЪЯВЛЕНИЯ МЕТОДОВ ---
    Camera();
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix(float width, float height);
    void processKeyboard(const char* direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void followTarget(float deltaTime);

private:
    void updateCameraVectors();
};