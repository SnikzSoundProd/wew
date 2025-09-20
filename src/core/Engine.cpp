#define STB_IMAGE_IMPLEMENTATION
#include "../vendor/stb_image.h"
#include "Engine.h"
#include "ResourceManager.h"
#include "SceneSerializer.h"
#include "Log.h" 
#include <iostream>
#include <memory>
#include <vector> // <--- ВОТ ОН, НАШ ГЛАВНЫЙ ГЕРОЙ
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// ImGui
#include "imgui_impl_opengl3.h"
#include <ImGuizmo.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Компоненты и графика
#include "PhysicsEngine.h"
#include "../components/RigidbodyComponent.h"
#include "../components/BoxColliderComponent.h"
#include "../components/PlayerControllerComponent.h"
#include "../components/AnimationComponent.h"
#include "../components/LightComponent.h"
#include "../components/MeshRenderer.h"
#include "../graphics/Shader.h"
#include "../graphics/Mesh.h"

Camera* g_Camera = nullptr;
float g_lastX = 0, g_lastY = 0;
bool g_firstMouse = true;

bool g_isSimulating = false;
// Callback-функция для мыши

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (g_firstMouse) { g_lastX = xpos; g_lastY = ypos; g_firstMouse = false; }
    float xoffset = xpos - g_lastX;
    float yoffset = g_lastY - ypos;
    g_lastX = xpos; g_lastY = ypos;
    if (g_isSimulating || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        if (g_Camera) g_Camera->processMouseMovement(xoffset, yoffset);
    }
}

void Engine::run() {
    init();
    mainLoop();
    cleanup();
}

void Engine::init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SimpleEngine by Bro", NULL, NULL);
    glfwMakeContextCurrent(m_window);
    glfwSetCursorPosCallback(m_window, mouse_callback);
    g_Camera = &m_camera;
    g_lastX = SCR_WIDTH / 2.0f;
    g_lastY = SCR_HEIGHT / 2.0f;

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    PhysicsEngine::GetInstance().init();
    m_outlineShader = ResourceManager::GetInstance().LoadShader("outline", "shaders/outline.vert", "shaders/outline.frag");

    Log::GetInstance().Info("Engine Initialized Successfully. Welcome, bro!");
    Log::GetInstance().Warn("This is a test warning.");
    Log::GetInstance().Error("And this is a test error.");

    // ИСПРАВЛЕНИЕ: Добавляем недостающие шейдеры
    auto simpleShader = ResourceManager::GetInstance().LoadShader("simple", "shaders/simple.vert", "shaders/simple.frag");
    auto animShader = ResourceManager::GetInstance().LoadShader("anim", "shaders/anim.vert", "shaders/anim.frag");
    auto backpackModel = ResourceManager::GetInstance().LoadModel("backpack", "models/backpack/backpack.obj");
    auto playerModel = ResourceManager::GetInstance().LoadModel("player", "models/player/tpose.fbx");

    auto sun = m_scene.createGameObject("Sun");
    sun->addComponent<MeshRenderer>(backpackModel, "backpack", "models/backpack/backpack.obj", simpleShader, "simple");
    sun->transform->scale = {0.2f, 0.2f, 0.2f};
    
    auto floor = m_scene.createGameObject("Floor");
    floor->transform->position = {0.0f, -2.0f, -5.0f};
    floor->transform->scale = {10.0f, 0.5f, 10.0f};
    floor->addComponent<MeshRenderer>(backpackModel, "backpack", "models/backpack/backpack.obj", simpleShader, "simple");
    auto floorRb = floor->addComponent<RigidbodyComponent>();
    floorRb->mass = 0.0f;
    auto floorCollider = floor->addComponent<BoxColliderComponent>();
    floorCollider->size = floor->transform->scale;
    floorRb->init();
    
    auto backpackObject = m_scene.createGameObject("Backpack");
    backpackObject->transform->position = {0.0f, 2.0f, -5.0f};
    backpackObject->addComponent<MeshRenderer>(backpackModel, "backpack", "models/backpack/backpack.obj", simpleShader, "simple");
    auto backpackRb = backpackObject->addComponent<RigidbodyComponent>();
    backpackRb->mass = 1.0f;
    backpackObject->addComponent<BoxColliderComponent>();
    auto controller = backpackObject->addComponent<PlayerControllerComponent>();
    controller->init(m_window); // Передаем окно явно
    controller->setCamera(&m_camera);
    backpackRb->init();
}

void Engine::mainLoop() {
    while (!glfwWindowShouldClose(m_window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        m_deltaTime = currentFrame - m_lastFrame;
        m_lastFrame = currentFrame;

        processInput(m_window);

        if (m_isSimulating) {
    m_physicsAccumulator += m_deltaTime;
    while (m_physicsAccumulator >= m_physicsTimeStep) {
        for (const auto& go : m_scene.gameObjects) {
            if (auto rb = go->getComponent<RigidbodyComponent>()) rb->beforePhysicsUpdate();
        }
        for (const auto& go : m_scene.gameObjects) {
            if (auto controller = go->getComponent<PlayerControllerComponent>()) controller->update(m_physicsTimeStep);
        }
        PhysicsEngine::GetInstance().update(m_physicsTimeStep);
        
        // ДОБАВЬТЕ ЭТО: Синхронизируем Transform после физики
        for (const auto& go : m_scene.gameObjects) {
            if (auto rb = go->getComponent<RigidbodyComponent>()) {
                if (rb->getBtRigidbody() && rb->getBtRigidbody()->getMass() > 0.0f) {
                    rb->syncWithPhysics();
                }
            }
        }
        
        m_physicsAccumulator -= m_physicsTimeStep;
    }
}
        
        if (m_camera.target) m_camera.followTarget(m_deltaTime);
        render();
        renderUI();
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}


void Engine::processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (m_isSimulating) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        g_firstMouse = false; 
        // В режиме симуляции клавиши WASD обрабатываются PlayerController'ом,
        // но мы все равно можем обрабатывать камеру здесь, если нужно
    } else {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) m_camera.processKeyboard("FORWARD", m_deltaTime);
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) m_camera.processKeyboard("BACKWARD", m_deltaTime);
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) m_camera.processKeyboard("LEFT", m_deltaTime);
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) m_camera.processKeyboard("RIGHT", m_deltaTime);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            g_firstMouse = true;
        }
    }
}

void Engine::render() {
    // Включаем нужные тесты OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // Настрока трафаретного буфера

    // Очищаем все буферы
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Получаем матрицы камеры
    glm::mat4 view = m_camera.getViewMatrix();
    glm::mat4 projection = m_camera.getProjectionMatrix((float)SCR_WIDTH, (float)SCR_HEIGHT);

    // --- 1. ИЩЕМ ИСТОЧНИК СВЕТА В СЦЕНЕ ---
    glm::vec3 lightPos = glm::vec3(0.0f);
    glm::vec3 lightColor = glm::vec3(1.0f);
    for (const auto& go : m_scene.gameObjects) {
        if (auto lightComp = go->getComponent<LightComponent>()) {
            lightPos = go->transform->position;
            lightColor = lightComp->color * lightComp->intensity;
            break;
        }
    }

    // --- 2. Первый проход: РИСУЕМ ВСЕ ОБЪЕКТЫ ---
    for (const auto& go : m_scene.gameObjects) {
        MeshRenderer* renderer = go->getComponent<MeshRenderer>();
        if (!renderer || !renderer->shader) continue; // Пропускаем объекты без рендерера

        // Настраиваем трафаретный буфер для выбранного объекта
        if (go.get() == m_selectedGameObject) {
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);
        } else {
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glStencilMask(0x00);
        }
        
        // Получаем матрицу модели
        glm::mat4 modelMatrix;
        RigidbodyComponent* rb = go->getComponent<RigidbodyComponent>();
        if (rb && m_isSimulating) {
            float alpha = m_physicsAccumulator / m_physicsTimeStep;
            modelMatrix = rb->getInterpolatedTransform(alpha);
        } else {
            modelMatrix = go->transform->getModelMatrix();
        }

        // Активируем шейдер и передаем все uniform-переменные
        renderer->shader->use();
        renderer->shader->setMat4("model", modelMatrix);
        renderer->shader->setMat4("view", view);
        renderer->shader->setMat4("projection", projection);
        
        // Передаем данные для света (если шейдер их поддерживает)
        renderer->shader->setVec3("lightPos", lightPos);
        renderer->shader->setVec3("viewPos", m_camera.position);
        renderer->shader->setVec3("lightColor", lightColor);

        // --- ГЛАВНАЯ ЧАСТЬ ДЛЯ АНИМАЦИИ ---
        // Если у объекта есть компонент анимации, передаем матрицы костей
        if (auto anim = go->getComponent<AnimationComponent>()) {
            if (anim->animator) {
                auto transforms = anim->animator->getFinalBoneMatrices();
                for (int i = 0; i < transforms.size(); ++i) {
                    // Имя uniform в шейдере: "finalBonesMatrices[i]"
                    renderer->shader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
                }
            }
        }
        
        // Рисуем объект
        renderer->draw(view, projection);
    }
    
    // --- 3. Второй проход: РИСУЕМ ОБВОДКУ ---
    if (m_selectedGameObject) {
        MeshRenderer* renderer = m_selectedGameObject->getComponent<MeshRenderer>();
        if (renderer && m_outlineShader) {
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            glDisable(GL_DEPTH_TEST);

            glm::mat4 modelMatrix;
            RigidbodyComponent* rb = m_selectedGameObject->getComponent<RigidbodyComponent>();
            if (rb && m_isSimulating) {
                 float alpha = m_physicsAccumulator / m_physicsTimeStep;
                 modelMatrix = rb->getInterpolatedTransform(alpha);
                 // Увеличиваем уже интерполированную матрицу, извлекая масштаб, увеличивая и собирая обратно
                 // (это сложная тема, пока просто увеличим масштаб в Transform)
                 Transform scaledTransform = *m_selectedGameObject->transform;
                 scaledTransform.scale *= 1.05f;
                 modelMatrix = scaledTransform.getModelMatrix(); // Временное упрощение
            } else {
                Transform scaledTransform = *m_selectedGameObject->transform;
                scaledTransform.scale *= 1.05f;
                modelMatrix = scaledTransform.getModelMatrix();
            }

            m_outlineShader->use();
            m_outlineShader->setMat4("model", modelMatrix);
            m_outlineShader->setMat4("view", view);
            m_outlineShader->setMat4("projection", projection);
            
            // Важно! Для обводки не нужно передавать кости,
            // т.к. outlineShader - простой шейдер
            renderer->model->draw(m_outlineShader);

            glStencilMask(0xFF);
            glEnable(GL_DEPTH_TEST);
        }
    }

    glDisable(GL_STENCIL_TEST);
}

void Engine::renderUI() {
    // Начинаем новый кадр ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    // --- ГЛАВНОЕ МЕНЮ ---
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save Scene")) {
                SceneSerializer serializer(m_scene);
                serializer.serialize("scene.json"); // Пока сохраняем в один файл
            }
            if (ImGui::MenuItem("Load Scene")) {
                SceneSerializer serializer(m_scene);
                serializer.deserialize("scene.json");
                m_selectedGameObject = nullptr; // Сбрасываем выбор
            }
            ImGui::EndMenu(); // Simulation
        }

            if (ImGui::BeginMenu("Simulation")) {
            if (ImGui::MenuItem(m_isSimulating ? "Stop" : "Play")) {
                m_isSimulating = !m_isSimulating;
                g_isSimulating = m_isSimulating; // <-- Обновляем глобальный флаг

                if (m_isSimulating) {
                    for (const auto& go : m_scene.gameObjects) {
                        if (go->getComponent<PlayerControllerComponent>()) {
                            m_camera.target = go.get();
                            break;
                        }
                    }
                } else {
                    m_camera.target = nullptr;
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();

    }

     if (ImGuizmo::IsOver() || ImGuizmo::IsUsing()) {
        ImGui::GetIO().WantCaptureMouse = false;
    }

    // --- 1. Окно иерархии сцены (ЕДИНСТВЕННЫЙ БЛОК) ---
    ImGui::Begin("Scene Hierarchy");

    // Кнопки "+" и "-"
    if (ImGui::Button("+")) {
        auto newObj = m_scene.createGameObject("New GameObject");
        auto backpackModel = ResourceManager::GetInstance().GetModel("backpack");
        auto simpleShader = ResourceManager::GetInstance().GetShader("simple");
        if (backpackModel && simpleShader) {
            newObj->addComponent<MeshRenderer>(backpackModel, "backpack", "models/backpack/backpack.obj", simpleShader, "simple");
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("-")) {
        if (m_selectedGameObject) {
            // Важно! Создаем копию указателя перед удалением, чтобы безопасно сбросить выбор
            GameObject* toDelete = m_selectedGameObject;
            m_selectedGameObject = nullptr;
            m_scene.destroyGameObject(toDelete);
        }
    }
    ImGui::Separator();

    // Перебираем объекты, чтобы их отобразить и обработать удаление
    // Создаем временный вектор для удаления, чтобы избежать проблем с итераторами
    GameObject* objectToDelete = nullptr;
    for (const auto& go : m_scene.gameObjects) {
        if (ImGui::Selectable(go->name.c_str(), m_selectedGameObject == go.get())) {
            m_selectedGameObject = go.get();
        }
        // Отмечаем объект для удаления, если нажата клавиша Delete
        if (m_selectedGameObject == go.get() && ImGui::IsItemHovered() && ImGui::IsKeyPressed(ImGuiKey_Delete)) {
            objectToDelete = go.get();
        }
    }

    // Если какой-то объект был отмечен для удаления, удаляем его
    if (objectToDelete) {
        if (m_selectedGameObject == objectToDelete) {
            m_selectedGameObject = nullptr;
        }
        m_scene.destroyGameObject(objectToDelete);
    }

    ImGui::End(); // <-- Завершаем окно иерархии

    // --- 2. Окно инспектора ---
    ImGui::Begin("Inspector");
    if (m_selectedGameObject) {
        ImGui::PushID(m_selectedGameObject);

        ImGui::Text("Name: %s", m_selectedGameObject->name.c_str());
        ImGui::Separator();
        
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            // ИСПОЛЬЗУЕМ -> ДЛЯ УКАЗАТЕЛЯ
            ImGui::DragFloat3("Position", &m_selectedGameObject->transform->position.x, 0.1f);
            ImGui::DragFloat3("Rotation", &m_selectedGameObject->transform->rotation.x, 1.0f);
            ImGui::DragFloat3("Scale", &m_selectedGameObject->transform->scale.x, 0.05f);
        }
        
        MeshRenderer* renderer = m_selectedGameObject->getComponent<MeshRenderer>();
        if (renderer) {
            if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Text("Model Ready"); 
            }
        }

        ImGui::PopID();
    } else {
        ImGui::Text("Select an object to inspect.");
    }
    ImGui::End(); // <-- Завершаем окно инспектора

    // --- 3. Рисуем GIZMO поверх всего (САМАЯ ВАЖНАЯ ЧАСТЬ) ---
    if (m_selectedGameObject) {
        // Настраиваем ImGuizmo на весь экран
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuiIO& io = ImGui::GetIO();
        if (io.DisplaySize.x > 0 && io.DisplaySize.y > 0) {
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

        glm::mat4 view = m_camera.getViewMatrix();
        glm::mat4 projection = m_camera.getProjectionMatrix(io.DisplaySize.x, io.DisplaySize.y);
        glm::mat4 modelMatrix = m_selectedGameObject->transform->getModelMatrix();

        ImGuizmo::Manipulate(
            glm::value_ptr(view), glm::value_ptr(projection),
            ImGuizmo::UNIVERSAL, ImGuizmo::LOCAL,
            glm::value_ptr(modelMatrix)
        );

        // Если Gizmo используется, обновляем transform
        if (ImGuizmo::IsUsing()) {
            glm::vec3 newPosition, newRotation, newScale;
            ImGuizmo::DecomposeMatrixToComponents(
                glm::value_ptr(modelMatrix), 
                glm::value_ptr(newPosition), 
                glm::value_ptr(newRotation), 
                glm::value_ptr(newScale)
            );
            m_selectedGameObject->transform->position = newPosition;
            m_selectedGameObject->transform->rotation = newRotation;
            m_selectedGameObject->transform->scale = newScale;
        }
    }
}

    m_consolePanel.render();
    // Рендерим все окна ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Engine::cleanup() {
    PhysicsEngine::GetInstance().shutdown();
    ResourceManager::GetInstance().Clear();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_window);
    glfwTerminate();
}