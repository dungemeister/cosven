#include <iostream>
#include <functional>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "demo.hpp"
#include "demo_mvc.hpp"
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
int Ring::rings_qty = 0;

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

void process_keys_input(GLFWwindow *window)
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

std::string get_source_from_file(std::string filepath){
    std::ifstream file(filepath, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        printf("Ошибка: не удалось открыть файл шейдера: %s\n", filepath.c_str());
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    std::string shaderCode = buffer.str();
    if (shaderCode.empty()) {
        printf("Ошибка: файл шейдера %s пуст\n", filepath.c_str());
        return "";
    }

    if (shaderCode.size() >= 3 && (unsigned char)shaderCode[0] == 0xEF &&
        (unsigned char)shaderCode[1] == 0xBB && (unsigned char)shaderCode[2] == 0xBF) {
        printf("Предупреждение: обнаружен UTF-8 BOM в %s, удаляем\n", filepath.c_str());
        shaderCode = shaderCode.substr(3);
    }

    printf("Файл шейдера %s загружен, размер: %zu байт\n", filepath.c_str(), shaderCode.size());
    // printf("Первые 100 символов:\n%.*s\n", (int)std::min(shaderCode.size(), size_t(100)), shaderCode.c_str());
    return shaderCode;
}

int demo_mvc(GLFWwindow *window){
    
    IMGUI_CHECKVERSION();

    DemoModel model;
    DemoViewModel view_model(window, model);
    DemoApplication app(view_model, window);

    app.run();

    return 0;
}

int imgui_system(GLFWwindow *window){
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

    // Загрузка и компиляция шейдера объектов
    std::string vertexShaderSrc =   get_source_from_file("shaders/satellite_vshader.glsl");
    std::string fragmentShaderSrc = get_source_from_file("shaders/satellite_fshader.glsl");
    if (vertexShaderSrc.empty() || fragmentShaderSrc.empty()) {
        printf("Ошибка: не удалось загрузить шейдеры объекта\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderCStr = vertexShaderSrc.c_str();
    glShaderSource(vertexShader, 1, &vertexShaderCStr, NULL);
    glCompileShader(vertexShader);
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(vertexShader, sizeof(infoLog), NULL, infoLog);
        printf("Vertex Shader Error: %s\n", infoLog);
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderСSrc = fragmentShaderSrc.c_str();
    glShaderSource(fragmentShader, 1, &fragmentShaderСSrc, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(fragmentShader, sizeof(infoLog), NULL, infoLog);
        printf("Fragment Shader Error: %s\n", infoLog);
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
        printf("Shader Program Error: %s\n", infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // Компиляция шейдера skybox
    std::string skyboxVertexShaderSrc = get_source_from_file("shaders/skybox_vshader.glsl").c_str();
    const char* skyboxVertexShaderCSrc = skyboxVertexShaderSrc.c_str();
    GLuint skyboxVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(skyboxVertexShader, 1, &skyboxVertexShaderCSrc, NULL);
    glCompileShader(skyboxVertexShader);
    glGetShaderiv(skyboxVertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(skyboxVertexShader, sizeof(infoLog), NULL, infoLog);
        printf("Skybox Vertex Shader Error: %s\n", infoLog);
    }

    std::string skyboxFragmentShaderSrc = get_source_from_file("shaders/skybox_fshader.glsl").c_str();
    const char* skyboxFragmentShaderCSrc = skyboxFragmentShaderSrc.c_str();
    GLuint skyboxFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(skyboxFragmentShader, 1, &skyboxFragmentShaderCSrc, NULL);
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

    //компиляция шейдера сегмента
    std::string segmentVertexShaderSrc = get_source_from_file("shaders/ringsegment_vshader.glsl").c_str();
    const char* segmentVertexShaderCSrc = segmentVertexShaderSrc.c_str();
    GLuint segmentVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(segmentVertexShader, 1, &segmentVertexShaderCSrc, NULL);
    glCompileShader(segmentVertexShader);
    glGetShaderiv(segmentVertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(segmentVertexShader, sizeof(infoLog), NULL, infoLog);
        printf("Skybox Vertex Shader Error: %s\n", infoLog);
    }

    std::string segmentFragmentShaderSrc = get_source_from_file("shaders/ringsegment_fshader.glsl").c_str();
    const char* segmentFragmentShaderCSrc = segmentFragmentShaderSrc.c_str();
    GLuint segmentFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(segmentFragmentShader, 1, &segmentFragmentShaderCSrc, NULL);
    glCompileShader(segmentFragmentShader);
    glGetShaderiv(segmentFragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(segmentFragmentShader, 512, NULL, infoLog);
        printf("Skybox Fragment Shader Error: %s\n", infoLog);
    }

    GLuint segmentShaderProgram = glCreateProgram();
    glAttachShader(segmentShaderProgram, segmentVertexShader);
    glAttachShader(segmentShaderProgram, segmentFragmentShader);
    glLinkProgram(segmentShaderProgram);
    glGetProgramiv(segmentShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(segmentShaderProgram, 512, NULL, infoLog);
        printf("Skybox Shader Program Error: %s\n", infoLog);
    }
    glDeleteShader(segmentVertexShader);
    glDeleteShader(segmentFragmentShader);

    camera.SetPosition(glm::vec3(0.0f, 0.0f, +30.0f));

    NewEarth earth("textures/earth.jpg");
    earth.PushEarth(glm::vec3(0.f, 0.f, 0.f));

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
    int rings_counter = 5;
    std::vector<Ring> rings;
    rings.reserve(rings_counter);
    for(int i = 0; i < rings_counter; i++){
        rings.emplace_back(i, "textures/body.jpg", "textures/wing.jpg");
    }

    for(auto& ring: rings){
        ring.updateRingsAngle();
        ring.pushSatellites(sats_counter);
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
        
        process_keys_input(window);
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

        for(auto& ring: rings)
        {
            if (satellite_movement)
                ring.rotateRing(sats_movespeed);

            if(is_orbital_camera)
                ring.render(shaderProgram, segmentShaderProgram, orbit_cam.GetViewMatrix(), orbit_cam.GetProjectionMatrix());
            else 
                ring.render(shaderProgram, segmentShaderProgram, camera.GetViewMatrix(), projection);
        }

        if(earth_movement){
            earth.Rotate(earth_movespeed, glm::vec3(0.f, 1.f, 0.f));
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

            {
                if(ImGui::SliderInt("Количество плоскостей", &rings_counter, 0, 30)){
                    std::cout << rings_counter << "\n";
                    if(rings.size() != rings_counter){
                        while(rings.size() != rings_counter){
                            if(rings_counter > rings.size()){
                                rings.emplace_back(rings.size(), "textures/body.jpg", "textures/wing.jpg");
                            }
                            else{
                                rings.pop_back();
                            }
                        }
                        for(auto& ring: rings){
                            ring.updateRingsAngle();
                            ring.pushSatellites(sats_counter);

                        }
                    }
                }
            }   

            if(ImGui::Button("Движение Земли")) earth_movement ^= true;
            ImGui::SliderFloat("Скорость вращения Земли", &earth_movespeed, 0.0, 15.0f, "%.1f");

            ImGui::SeparatorText("Спутники");
            if(ImGui::SliderInt("Спутники плоскости", &sats_counter, 0, 30) && (rings.size() > 0)){
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