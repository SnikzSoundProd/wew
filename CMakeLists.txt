# --- 1. Определение проекта и стандарта C++ ---
# Минимальная требуемая версия CMake
cmake_minimum_required(VERSION 3.16)

# Название нашего проекта и язык
project(SimpleEngine LANGUAGES CXX)

if(MSVC)
    # Говорим, что наши .cpp/.h файлы в UTF-8
    add_compile_options(/source-charset:utf-8)
    # Говорим, что исполняемый файл и ВЫВОД КОМПИЛЯТОРА тоже должны быть в UTF-8
    add_compile_options(/execution-charset:utf-8)
endif()

# Устанавливаем стандарт C++17 и требуем его поддержки
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# --- 2. Интеграция с Vcpkg (САМАЯ ВАЖНАЯ ЧАСТЬ) ---
# Эта строка говорит CMake, где искать библиотеки, установленные через vcpkg.
# !!! ЗАМЕНИ "C:/l/vcpkg" НА СВОЙ РЕАЛЬНЫЙ ПУТЬ К ПАПКЕ vcpkg !!!
set(CMAKE_TOOLCHAIN_FILE "C:/l/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")


# --- 3. Поиск необходимых библиотек ---
# Ищем конфигурационные файлы для каждой библиотеки.
# vcpkg сам их создал, поэтому CMake их легко найдет.
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(imguizmo CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Bullet CONFIG REQUIRED)

# --- 4. Сбор исходных файлов проекта ---
# Эта команда автоматически находит все файлы с расширением .cpp
# в папке src и всех ее подпапках. Так не придется добавлять каждый файл вручную.
add_executable(SimpleEngine
    src/main.cpp
    src/core/Engine.cpp
    src/core/ResourceManager.cpp
    src/core/Scene.cpp
    src/core/Transform.cpp              # <-- НОВЫЙ
    src/components/MeshRenderer.cpp     # <-- НОВЫЙ
    src/core/SceneSerializer.cpp
    src/graphics/Shader.cpp   # <--- ВОТ ОН! ГЛАВНЫЙ ГЕРОЙ!
    src/graphics/Mesh.cpp     # <--- И его друг, на всякий случай
    src/graphics/Texture2D.cpp
    src/graphics/Model.cpp
    src/core/PhysicsEngine.cpp # <-- НОВЫЙ
    src/components/RigidbodyComponent.cpp
    src/components/PlayerControllerComponent.cpp
    src/animation/Animation.cpp
    src/animation/Animator.cpp
    src/animation/Bone.cpp
    src/graphics/Camera.cpp
)


# --- 6. Указание путей к заголовочным файлам (.h) ---
# Говорим компилятору, что все наши .h файлы лежат в папке src,
# чтобы он мог найти их по #include <core/Engine.h> и т.д.
target_include_directories(SimpleEngine PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)


# --- 7. Линковка (подключение) библиотек ---
# "Приклеиваем" скачанные библиотеки к нашему проекту. Без этого
# компилятор не будет знать, где искать функции вроде glfwInit() или glClear().
target_link_libraries(SimpleEngine PRIVATE
    # Эти библиотеки используют современный синтаксис с "::"
    glm::glm
    glad::glad
    imgui::imgui

    # А эта остается как есть, vcpkg ее так и называет
    glfw
    assimp::assimp
    imguizmo::imguizmo
    nlohmann_json::nlohmann_json
    ${BULLET_LIBRARIES}
)


# --- 8. Копирование ресурсов (шейдеров) ---
# Эта команда решает проблему, когда .exe не может найти шейдеры.
# Она будет автоматически копировать папку /shaders в папку со сборкой
# КАЖДЫЙ РАЗ после успешной компиляции.
add_custom_command(TARGET SimpleEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
    "$<TARGET_FILE_DIR:SimpleEngine>/shaders"
    COMMENT "Copying shaders to build directory"
)

# ДОБАВЬ ЭТОТ БЛОК: Копируем ресурсы (текстуры)
add_custom_command(TARGET SimpleEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/textures"
    "$<TARGET_FILE_DIR:SimpleEngine>/textures"
    COMMENT "Copying textures to build directory"
)

# ДОБАВЬ ЭТОТ БЛОК для моделей!
add_custom_command(TARGET SimpleEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/models"
    "$<TARGET_FILE_DIR:SimpleEngine>/models"
    COMMENT "Copying models to build directory"
)