#include <iostream>
#include <functional>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "demo.hpp"
#include "ring.hpp"
#include "earth.hpp"
#include "skybox.hpp"
#include "nfd.h"

#include <imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

extern FreeCamera camera;
extern bool mouse_rotation;
extern double g_xpos, g_ypos;
extern int SAT_INDEX;
extern uint32_t g_width, g_height;
extern float deltatime, lastFrame;

extern float lastX , lastY;
extern bool firstMouse;

const FreeCamera camera_scene_spot1 = FreeCamera(glm::vec3(0.0f, 0.0f, 20.0f), glm::vec3(0.0f, 1.0f, 0.0f), {}, {});

OrbitCamera orbit_cam;

bool imgui_hovered = false;

bool camera_movement = false;
static bool is_orbital_camera = true;

void glfw_scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    if(imgui_hovered) return;
    if(is_orbital_camera){
        orbit_cam.IncreaseDistance(yoffset);
    }
    else{
        camera.ProcessMouseScroll(static_cast<float> (yoffset));
    }
}

void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(imgui_hovered) return;
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        mouse_rotation = false;
    }
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        glfwGetCursorPos(window, &g_xpos, &g_ypos);
        mouse_rotation = true;

    }
}

void glfw_mouse_cursor_callback(GLFWwindow *window, double xpos, double ypos)
{

    float xposFloat = static_cast<float>(xpos);
    float yposFloat = static_cast<float>(ypos);
    if(firstMouse)
    {
        lastX = xposFloat;
        lastY = yposFloat;
        firstMouse = false;
    }
    float xoffset = xposFloat - lastX;
    float yoffset = lastY - yposFloat;

    lastX = xposFloat;
    lastY = yposFloat;
    if(!mouse_rotation)
        return;
    if(is_orbital_camera){
        orbit_cam.UpdateAngle(xoffset);
    }
    else {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }

}

void processKeysInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        camera = camera_scene_spot1;
    }

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)

    {
        camera.ProcessKeyboard(FORWARD, deltatime);
    }

    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltatime);

    }

    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltatime);
    }

    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltatime);

    }
    return;
}

int imgui_system(GLFWwindow *window){
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    int g_width = 1920;
    int g_height = 1080;
    auto time = glfwGetTime();

    // glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, glfw_scroll_callback);
    glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);
    glfwSetCursorPosCallback(window, glfw_mouse_cursor_callback);

    glEnable(GL_DEPTH_TEST);

    projection = glm::perspective(glm::radians(45.0f), (float)g_width / (float)g_height, 0.1f, 100.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(); 

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Загрузка шрифта с поддержкой кириллицы (например, DejaVuSans)
    io.Fonts->AddFontFromFileTTF("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    io.Fonts->Build();

    const char* vertexShaderSrc = R"(
        #version 450 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec2 aTexCoord;
        layout(location = 2) in vec3 instancePos;
        uniform mat4 mvp;
        uniform mat4 offsetModel;
        out vec2 TexCoord;
        void main() {
            gl_Position = mvp * (offsetModel * vec4(aPos, 1.0) + vec4(instancePos, 0.0));
            TexCoord = aTexCoord;
        }
    )";
    const char* fragmentShaderSrc = R"(
        #version 450 core
        in vec2 TexCoord;
        out vec4 FragColor;
        uniform sampler2D texture1;
        void main() {
            FragColor = texture(texture1, TexCoord);
        }
    )";

    // Шейдер для skybox
    const char* skyboxVertexShaderSrc = R"(
        #version 450 core
        layout(location = 0) in vec3 aPos;
        uniform mat4 mvp;
        out vec3 TexCoords;
        void main() {
            TexCoords = aPos;
            gl_Position = mvp * vec4(aPos, 1.0);
        }
    )";
    const char* skyboxFragmentShaderSrc = R"(
        #version 450 core
        in vec3 TexCoords;
        out vec4 FragColor;
        uniform samplerCube skybox;
        void main() {
            FragColor = texture(skybox, TexCoords);
        }
    )";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
    glCompileShader(vertexShader);
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("Vertex Shader Error: %s\n", infoLog);
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("Fragment Shader Error: %s\n", infoLog);
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("Shader Program Error: %s\n", infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // Компиляция шейдера skybox
    GLuint skyboxVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(skyboxVertexShader, 1, &skyboxVertexShaderSrc, NULL);
    glCompileShader(skyboxVertexShader);
    glGetShaderiv(skyboxVertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(skyboxVertexShader, 512, NULL, infoLog);
        printf("Skybox Vertex Shader Error: %s\n", infoLog);
    }

    GLuint skyboxFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(skyboxFragmentShader, 1, &skyboxFragmentShaderSrc, NULL);
    glCompileShader(skyboxFragmentShader);
    glGetShaderiv(skyboxFragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(skyboxFragmentShader, 512, NULL, infoLog);
        printf("Skybox Fragment Shader Error: %s\n", infoLog);
    }

    GLuint skyboxShaderProgram = glCreateProgram();
    glAttachShader(skyboxShaderProgram, skyboxVertexShader);
    glAttachShader(skyboxShaderProgram, skyboxFragmentShader);
    glLinkProgram(skyboxShaderProgram);
    glGetProgramiv(skyboxShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(skyboxShaderProgram, 512, NULL, infoLog);
        printf("Skybox Shader Program Error: %s\n", infoLog);
    }
    glDeleteShader(skyboxVertexShader);
    glDeleteShader(skyboxFragmentShader);

    camera.SetPosition(glm::vec3(0.0f, 0.0f, +30.0f));

    NewEarth earth("textures/earth.jpg");
    earth.PushEarth({0.f,0.f,0.f});

    std::vector<std::string> faces = {
        "test_skybox/right.jpg",
        "test_skybox/left.jpg",
        "test_skybox/top.jpg",
        "test_skybox/bottom.jpg",
        "test_skybox/front.jpg",
        "test_skybox/back.jpg"
    };
    NewSkybox skybox(faces);

    int sats_counter = 20;
    std::vector<Ring> rings;
    for(int i = 0; i < 5; i++){
        Ring r(i, "textures/body.jpg", "textures/wing.jpg");
        r.pushSatellites(sats_counter);
        rings.push_back(r);
    }

    while(!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        using namespace std::chrono_literals;

        auto end = std::chrono::steady_clock::now() + 16ms;
        static float sats_movespeed = 1.0f;
        static bool satellite_movement = false;
        static bool earth_movement = false;
        static float earth_movespeed = 1.0f;
        static float orbit_camera_angle_coef = 0.0f;
        static float orbit_camera_distance = 15.0f;
        static float widgets_width = 600.0f;
        
        static std::string selected_file; // Для хранения пути выбранного файла

        time = glfwGetTime();
        glfwPollEvents();
        
        processKeysInput(window);
        if (io.WantCaptureMouse) imgui_hovered = true;
        else imgui_hovered = false;
        if(camera_movement) orbit_cam.Update(orbit_camera_angle_coef);
        if(is_orbital_camera)
        {
            projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(g_width) / g_height, 0.1f, 100.f);
            view = orbit_cam.GetViewMatrix();
        }
        else{
            projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(g_width) / g_height, 0.1f, 100.f);
            view = camera.GetViewMatrix();
        }

        skybox.Render(skyboxShaderProgram,view, projection);
        earth.Render(shaderProgram, view, projection);

        for(auto it = rings.begin();it != rings.end(); ++it)
        {
            if (satellite_movement)
                it->rotateRing(sats_movespeed);

            if(is_orbital_camera)
                it->render(shaderProgram, orbit_cam.GetViewMatrix(), orbit_cam.GetProjectionMatrix());
            else 
                it->render(shaderProgram, camera.GetViewMatrix(), projection);
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImVec2 pos = {};
            ImVec2 size = {widgets_width, 600};

            ImGui::SetNextWindowPos(pos);
            ImGui::SetNextWindowSize(size);

            ImGui::Begin("Контроллер плоскостей");
                        
            if(ImGui::Button("Движение Земли")) earth_movement ^= true;
            ImGui::SliderFloat("Скорость вращения Земли", &earth_movespeed, 0.0, 15.0f, "%.1f");

            ImGui::SeparatorText("Спутники");
            if(ImGui::SliderInt("Спутники плоскости", &sats_counter, 0, 30)){
                std::cout << sats_counter << "\n";
                auto size = rings.at(0).getSatellitesQty();
                if(size != sats_counter){
                    for(int ring_index = 0; ring_index < rings.size(); ring_index++){
                        rings[ring_index].pushSatellites(sats_counter);
                    }
                    size = rings.at(0).getSatellitesQty();
                }

            }
            if(ImGui::Button("Движение спутников")) satellite_movement ^= true;
            ImGui::SliderFloat("Скорость спутников", &sats_movespeed, -2.0, 2.0f, "%.1f");

            
            ImGui::SeparatorText("Параметры камеры");
            if(ImGui::Button("Сменить камеру")) is_orbital_camera ^= true;
            if(is_orbital_camera) {
                ImGui::SeparatorText("Орбитальная камера");
                if(ImGui::Button("Движение камеры")) camera_movement ^= true;
                ImGui::SliderFloat("Скорость вращения камеры", &orbit_camera_angle_coef, -0.3, 0.3f, "%.2f");

                // if(ImGui::SliderFloat("Дальность камеры", &orbit_camera_distance, 0.0, 100.0f, "%.1f")) orbit_cam.setDistance(orbit_camera_distance);
                ImGui::LabelText("Орбитальная камера", "Текущая камера:");
                auto pos = orbit_cam.GetPosition();
                ImGui::Text("x: %.2f", pos.x);
                ImGui::SameLine(); ImGui::Text("y: %.2f", pos.y);
                ImGui::SameLine(); ImGui::Text("z: %.2f", pos.z);
                ImGui::SameLine(); ImGui::Text("dist: %.2f", orbit_cam.GetDistance());
            }
            else{
                ImGui::LabelText("Свободная камера", "Текущая камера:");
                ImGui::Text("x: %.2f", camera.Position.x);
                ImGui::SameLine(); ImGui::Text("y: %.2f", camera.Position.y);
                ImGui::SameLine(); ImGui::Text("z: %.2f", camera.Position.z);
                {
                    ImVec2 pos = {0, 600};
                    ImVec2 size = {widgets_width, 300};
                    ImGui::SetNextWindowPos(pos);
                    ImGui::SetNextWindowSize(size);

                    ImGui::Begin("Сцены");
                    static int selected_item = -1; // Индекс выбранного элемента
                    static std::vector<std::string> items = {"Default", "Spot1"};
                    if (ImGui::BeginListBox("##listbox", ImVec2(0, 100))) { // Размер списка
                        for (int i = 0; i < items.size(); ++i) {
                            bool is_selected = (selected_item == i);
                            if (ImGui::Selectable(items[i].c_str(), is_selected)) {
                                selected_item = i; // Обновление выбранного элемента
                                std::string scene = items[selected_item];
                            }
                        }
                        ImGui::EndListBox();
                    }
                    if (selected_item != -1) {
                        std::string scene = items[selected_item];
                        ImGui::Text("Текущая сцена: %s", scene.c_str());

                    }
                    ImGui::End();
                }
            }
            ImGui::Text("TODO:\n"
                        "#Добавить управление плоскостями\n"
                        "#Добавить Землю с текстурой\n");

            // auto texture = earth->getTexture();
            // if (ImGui::ImageButton("Earth_texture_button", (void*)(intptr_t)texture, ImVec2(100, 100), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
            //     nfdchar_t *outPath = NULL;
            //     nfdresult_t result = NFD_OpenDialog(NULL, NULL, &outPath);
            //     if (result == NFD_OKAY) {
            //         selected_file = outPath;
            //         free(outPath);
            //         earth->deleteShaderTexture();
            //         earth->loadShaderTexture(selected_file);

            //     } else if (result == NFD_CANCEL) {
            //         selected_file = "Отмена";
            //     } else {
            //         selected_file = "Ошибка";
            //     }
            // }
            ImGui::End();
        }
        

        ImGui::Render();
        glClearColor(0.2f, 0.2f, 0.2f, 0.5f);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        std::this_thread::sleep_until(end);
        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}