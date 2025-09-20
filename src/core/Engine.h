#pragma once
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Scene.h"
#include "../graphics/Camera.h"
#include "../graphics/Shader.h"
#include "../editor/ConsolePanel.h"

class Engine {
public:
    void run();

private:
    void init();
    void mainLoop();
    void cleanup();

    void render();
    void renderUI();

    GLFWwindow* m_window = nullptr;
    Scene m_scene;
    Camera m_camera;
    
    const unsigned int SCR_WIDTH = 1280;
    const unsigned int SCR_HEIGHT = 720;

    GameObject* m_selectedGameObject = nullptr;

    float m_lastX = SCR_WIDTH / 2.0f;
    float m_lastY = SCR_HEIGHT / 2.0f;
    bool m_firstMouse = true;
    float m_deltaTime = 0.0f;
    float m_lastFrame = 0.0f;

    void processInput(GLFWwindow *window);
    std::shared_ptr<Shader> m_outlineShader;
    bool m_isSimulating = false;

    ConsolePanel m_consolePanel;

    float m_physicsAccumulator = 0.0f; // <-- Наша "копилка"
    const float m_physicsTimeStep = 1.0f / 60.0f; // <-- Шаг симуляции (60 раз в секунду)
};