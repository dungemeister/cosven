#ifndef DEMO_MVC_HPP
#define DEMO_MVC_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GLFW/glfw3.h"

#include <functional>
#include <memory>
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>
#include <vector>

#include "camera.hpp"
#include "orbit_camera.hpp"
#include "ring.hpp"
#include "earth.hpp"
#include "skybox.hpp"

#include <imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

class DemoModel{
public:
    int g_width = 1920;
    int g_height = 1080;

    double mouse_xpos;
    double mouse_ypos;
    float mouse_lastx;
    float mouse_lasty;
    bool mouse_rotation;
    bool mouse_first;
    
    bool is_orbital_camera = true;

    float deltatime = 0.08;

    bool satellite_movement = false;
    bool planets_movement = false;
    bool planets_rotation = false;
    float satellite_speed = 0.2f;
    int rings_counter = 5;
    int sats_counter = 20;

    glm::vec3 default_cam_position = glm::vec3(0.0f, 0.0f, 10.0f);
    OrbitCamera orbit_cam;
    FreeCamera camera = FreeCamera(default_cam_position, glm::vec3(0.0f, 1.0f, 0.0f), {}, {});
    bool camera_movement = false;

    glm::mat4 view = glm::translate(view, glm::vec3(0.0f, 0.0f, -50.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)g_width / (float)g_height, 0.1f, 200.0f);

    std::vector<Ring> rings;
    std::vector<NewEarth> planets;
    std::vector<NewSkybox> skyboxes;

    float mercury_distance = 20.0f;
    float earth_distance = mercury_distance + 15.f;
    float mars_distance = earth_distance + 20.f;

    DemoModel(){

        auto sun = NewEarth("textures/sun.jpg", 12.f);
        sun.PushEarth(glm::vec3(0.f, 0.f, 0.f));

        auto mercury = NewEarth("textures/mercury.jpg", 4.0f);
        mercury.PushEarth(glm::vec3(0.f, 0.f, mercury_distance));

        auto earth = NewEarth("textures/earth.jpg");
        earth.PushEarth(glm::vec3(0.f, 0.f, earth_distance));

        auto moon = NewEarth("textures/moon.jpg", 1.f);
        moon.PushEarth(glm::vec3(0.f, 0.f, earth_distance + 10.f));

        auto mars = NewEarth("textures/mars.jpg", 5.f);
        mars.PushEarth(glm::vec3(0.f, 0.f, mars_distance));

        planets.push_back(sun);
        planets.push_back(mercury);
        planets.push_back(earth);
        planets.push_back(mars);
        planets.push_back(moon);

        for(int i = 0; i < rings_counter; i++){
            rings.emplace_back(i, "textures/body.jpg", "textures/wing.jpg", glm::vec3(0.f, 0.f, earth_distance));
        }

        for(auto& ring: rings){
            ring.updateRingsAngle();
            ring.pushSatellites(sats_counter);
        }
        std::vector<std::string> faces = {
            "test_skybox/right.jpg",
            "test_skybox/left.jpg",
            "test_skybox/top.jpg",
            "test_skybox/bottom.jpg",
            "test_skybox/front.jpg",
            "test_skybox/back.jpg"
        };
        skyboxes.emplace_back(faces);

    }

};

struct ImguiModel{
    bool imgui_hovered = false;
    ImVec2 main_widget_size = {400.f, 400.f};
    ImVec2 main_widget_pos = {0.f, 0.f};
    ImVec2 camera_widget_size = {400.f, 400.f};
    ImVec2 camera_widget_pos = {0.f, 400.f};
    ImVec2 planets_widget_size = {400.f, 400.f};
    ImVec2 planets_widget_pos = {0.f, 800.f};
};

struct ShadersModel{
    GLuint satellite_shader_program = 0;
    GLuint segment_shader_program = 0;
    GLuint earth_shader_program = 0;
    GLuint skybox_shader_program = 0;
    GLuint border_shader_program = 0;

    ShadersModel(){
        satelliteShadersInit();
        segmentShaderProgramInit();
        skyboxShaderInit();
        borderShaderInit();
    }

    void satelliteShadersInit(){
       // Загрузка и компиляция шейдера объектов
        std::string vertexShaderSrc =   get_source_from_file("shaders/satellite_vshader.glsl");
        std::string fragmentShaderSrc = get_source_from_file("shaders/satellite_fshader.glsl");
        if (vertexShaderSrc.empty() || fragmentShaderSrc.empty()) {
            printf("Ошибка: не удалось загрузить шейдеры объекта\n");
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

        satellite_shader_program = glCreateProgram();
        glAttachShader(satellite_shader_program, vertexShader);
        glAttachShader(satellite_shader_program, fragmentShader);
        glLinkProgram(satellite_shader_program);
        glGetProgramiv(satellite_shader_program, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[1024];
            glGetProgramInfoLog(satellite_shader_program, sizeof(infoLog), NULL, infoLog);
            printf("Shader Program Error: %s\n", infoLog);
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void segmentShaderProgramInit(){
        //компиляция шейдера сегмента
        GLint success;
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

        segment_shader_program = glCreateProgram();
        glAttachShader(segment_shader_program, segmentVertexShader);
        glAttachShader(segment_shader_program, segmentFragmentShader);
        glLinkProgram(segment_shader_program);
        glGetProgramiv(segment_shader_program, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(segment_shader_program, 512, NULL, infoLog);
            printf("Skybox Shader Program Error: %s\n", infoLog);
        }
        glDeleteShader(segmentVertexShader);
        glDeleteShader(segmentFragmentShader);
    }

    void skyboxShaderInit(){
        // Компиляция шейдера skybox
        GLint success;
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

        skybox_shader_program = glCreateProgram();
        glAttachShader(skybox_shader_program, skyboxVertexShader);
        glAttachShader(skybox_shader_program, skyboxFragmentShader);
        glLinkProgram(skybox_shader_program);
        glGetProgramiv(skybox_shader_program, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(skybox_shader_program, 512, NULL, infoLog);
            printf("Skybox Shader Program Error: %s\n", infoLog);
        }
        glDeleteShader(skyboxVertexShader);
        glDeleteShader(skyboxFragmentShader);
    }

    void borderShaderInit(){
        // Компиляция шейдера skybox
        GLint success;
        std::string borderVertexShaderSrc = get_source_from_file("shaders/satellite_vshader.glsl").c_str();
        const char* borderVertexShaderCSrc = borderVertexShaderSrc.c_str();
        GLuint borderVertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(borderVertexShader, 1, &borderVertexShaderCSrc, NULL);
        glCompileShader(borderVertexShader);
        glGetShaderiv(borderVertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[1024];
            glGetShaderInfoLog(borderVertexShader, sizeof(infoLog), NULL, infoLog);
            printf("Skybox Vertex Shader Error: %s\n", infoLog);
        }

        std::string borderFragmentShaderSrc = get_source_from_file("shaders/border_fshader.glsl").c_str();
        const char* borderFragmentShaderCSrc = borderFragmentShaderSrc.c_str();
        GLuint borderFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(borderFragmentShader, 1, &borderFragmentShaderCSrc, NULL);
        glCompileShader(borderFragmentShader);
        glGetShaderiv(borderFragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(borderFragmentShader, 512, NULL, infoLog);
            printf("Skybox Fragment Shader Error: %s\n", infoLog);
        }

        border_shader_program = glCreateProgram();
        glAttachShader(border_shader_program, borderVertexShader);
        glAttachShader(border_shader_program, borderFragmentShader);
        glLinkProgram(border_shader_program);
        glGetProgramiv(border_shader_program, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(border_shader_program, 512, NULL, infoLog);
            printf("Skybox Shader Program Error: %s\n", infoLog);
        }
        glDeleteShader(borderVertexShader);
        glDeleteShader(borderFragmentShader);
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
};

class DemoViewModel{
public:
    DemoViewModel(GLFWwindow *window, DemoModel& model):
    m_window(window),
    m_model(model){
        // Сохраняем указатель на объект в окне
        glfwSetWindowUserPointer(m_window, this);

        // Устанавливаем статическую функцию как callback
        glfwSetScrollCallback(m_window, scrollCallback);
        // Устанавливаем статическую функцию как callback
        glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
        // Устанавливаем статическую функцию как callback
        glfwSetCursorPosCallback(m_window, cursorPosCallback);
    }
    void handleKeysInput(){
        if(glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(m_window, GL_TRUE);
        }

        if(glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            // camera = camera_scene_spot1;
        }

        if(glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)

        {
            m_model.camera.ProcessKeyboard(FORWARD, m_model.deltatime);
        }

        if(glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        {
            m_model.camera.ProcessKeyboard(BACKWARD, m_model.deltatime);

        }

        if(glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
        {
            m_model.camera.ProcessKeyboard(RIGHT, m_model.deltatime);
        }

        if(glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        {
            m_model.camera.ProcessKeyboard(LEFT, m_model.deltatime);

        }
        return;
    }

    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        DemoViewModel* view_model = static_cast<DemoViewModel*>(glfwGetWindowUserPointer(window));
        if(view_model){

            if(view_model->getImguiHovered()) return;
            if(view_model->m_model.is_orbital_camera) view_model->m_model.orbit_cam.IncreaseDistance(yoffset);
            else view_model->m_model.camera.ProcessMouseScroll(static_cast<float> (yoffset));
        }

    }
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
        DemoViewModel* view_model = static_cast<DemoViewModel*>(glfwGetWindowUserPointer(window));
        if(view_model){

            if(view_model->getImguiHovered()) return;
            if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
            {
                view_model->m_model.mouse_rotation = false;
                
            }
            if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
            {
                glfwGetCursorPos(window, &view_model->m_model.mouse_xpos, &view_model->m_model.mouse_ypos);
                view_model->m_model.mouse_rotation = true;

            }
        }
    }
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos){
        DemoViewModel* view_model = static_cast<DemoViewModel*>(glfwGetWindowUserPointer(window));
        if(view_model){
            float xposFloat = static_cast<float>(xpos);
            float yposFloat = static_cast<float>(ypos);
            if(view_model->m_model.mouse_first)
            {
                view_model->m_model.mouse_lastx = xposFloat;
                view_model->m_model.mouse_lasty = yposFloat;
                view_model->m_model.mouse_first = false;
            }
            float xoffset = xposFloat - view_model->m_model.mouse_lastx;
            float yoffset = view_model->m_model.mouse_lasty - yposFloat;

            view_model->m_model.mouse_lastx = xposFloat;
            view_model->m_model.mouse_lasty = yposFloat;
            if(!view_model->m_model.mouse_rotation)
                return;
            if(view_model->m_model.is_orbital_camera){
                view_model->m_model.orbit_cam.UpdateAngle(xoffset);
            }
            else {
                view_model->m_model.camera.ProcessMouseMovement(xoffset, yoffset);
            }
        }
    }
    ImguiModel getImguiData(){ return m_imgui_model; }

    void setImguiMainWidgetSize( const ImVec2& size ) { m_imgui_model.main_widget_size = size;}
    void setImguiSecWidgetSize( const ImVec2& size ) { m_imgui_model.camera_widget_size = size;}

    int getWindowWidth() { return m_model.g_width; }
    int getWindowHeight() { return m_model.g_height; }
    
    bool getImguiHovered() { return m_imgui_model.imgui_hovered; }
    void setImguiHovered(bool state) { m_imgui_model.imgui_hovered = state; }

    size_t getRingsQuantity() { return m_model.rings.size(); }
    size_t getSatellitesQuantity() { return m_model.sats_counter; }
    void setSatellitesQuantity(int sats_qty) { m_model.sats_counter = sats_qty; }

    void setPlanetsMovement(bool state) { m_model.planets_movement = state; }
    bool getPlanetsMovement() { return m_model.planets_movement; }

    void setPlanetsRotation(bool state) { m_model.planets_rotation = state; }
    bool getPlanetsRotation() { return m_model.planets_rotation; }

    void setSatellitesMovement(bool state) { m_model.satellite_movement = state; }
    bool getSatellitesMovement() { return m_model.satellite_movement; }

    glm::vec3 getCameraPosition() { return m_model.is_orbital_camera ? m_model.orbit_cam.GetPosition(): m_model.camera.Position; }
    void setCameraPosition(glm::vec3 pos) { m_model.orbit_cam.setPosition(pos); }
    float getDistance() { return m_model.is_orbital_camera ? m_model.orbit_cam.GetDistance(): m_model.camera.GetDistance(); }
    
    glm::vec3   getCameraTarget() { return m_model.orbit_cam.getTarget(); }
    void        setCameraTarget(const glm::vec3& target ) { m_model.orbit_cam.setTarget(target); }
    
    void setCameraMovement(bool state) { m_model.camera_movement = state; }
    bool getCameraMovement() { return m_model.camera_movement; }

    void cameraMove(float angle) { m_model.orbit_cam.Update(angle); }

    glm::vec3 getPlanetCenterVec(int planet_index) { return m_model.planets[planet_index].GetCenterCoords(); }

    void updateRings(int new_count){
        while(m_model.rings.size() != new_count){
            if(new_count > m_model.rings.size()){
                m_model.rings.emplace_back(m_model.rings.size(), "textures/body.jpg",
                                           "textures/wing.jpg", glm::vec3(0.f, 0.f, m_model.earth_distance));
            }
            else{
                m_model.rings.pop_back();
            }
        }
        for(auto& ring: m_model.rings){
            ring.updateRingsAngle();
            ring.pushSatellites(m_model.sats_counter);

        }
    }

    void updateSatellites(int new_counter){
        if(getSatellitesQuantity() != new_counter){
            for(auto& ring: m_model.rings){
                ring.pushSatellites(new_counter);
            }
            setSatellitesQuantity(new_counter);
        }
    }

    void render(){
        updateModel();
        renderSkybox(); //Важно рендерить сначала skybox со своими opengl параметрами
        renderPlanets();
        renderRings();
    }

    void renderSkybox(){

        m_model.skyboxes[0].Render(m_shaders_model.skybox_shader_program,
                      m_model.orbit_cam.GetViewMatrix(), m_model.orbit_cam.GetProjectionMatrix());
    }

    void renderRings(){
        for(auto& ring: m_model.rings)
        {
            if (m_model.satellite_movement)
                ring.rotateRing(m_model.satellite_speed);

            if(m_model.is_orbital_camera)
                ring.render(m_shaders_model.satellite_shader_program, m_shaders_model.segment_shader_program,
                 m_model.orbit_cam.GetViewMatrix(), m_model.orbit_cam.GetProjectionMatrix(), m_shaders_model.border_shader_program);
            // else 
            //     ring.render(m_shaders_model.satellite_shader_program, segmentShaderProgram, m_model.camera.GetViewMatrix(), projection);
        }
    }

    void renderPlanets(){
        for(auto& planet: m_model.planets){
            if(m_model.planets_rotation) planet.Rotate(0.1f, glm::vec3(0.f, 1.f, 0.f));
            if(m_model.planets_movement) planet.Move();

            planet.Render(m_shaders_model.satellite_shader_program,
                          m_model.orbit_cam.GetViewMatrix(), m_model.orbit_cam.GetProjectionMatrix());
        }
    }

    void updateModel(){
        if(m_model.camera_movement) cameraMove(0.03f);
    }

    std::vector<std::string> getRingsDisplayNames() {
        std::vector<std::string> res;
        for(auto& ring: m_model.rings){
            res.push_back(ring.getDisplayName());
        }
        return res;
    }
    void selectRingByDisplayName(const std::string& display_name){
        for(auto& ring: m_model.rings){
            auto ring_name = ring.getDisplayName();
            if(ring_name == display_name){
                ring.setSelected(true);
            }
            else{
                ring.setSelected(false);
            }
        }
    }
private:
    GLFWwindow *m_window;
    DemoModel& m_model;
    ImguiModel m_imgui_model;
    ShadersModel m_shaders_model;


};

class DemoApplication{
public:
    DemoApplication(DemoViewModel& view_model, GLFWwindow *window);

    void run(){

        glEnable(GL_DEPTH_TEST);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        ImGui_ImplOpenGL3_Init(); 

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Загрузка шрифта с поддержкой кириллицы (например, DejaVuSans)
        io.Fonts->AddFontFromFileTTF("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
        io.Fonts->Build();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        // glEnable(GL_STENCIL_TEST);
        // glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        while(!glfwWindowShouldClose(m_window)){
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            glfwPollEvents();
            updateFPS();
            
            if (io.WantCaptureMouse) m_view_model.setImguiHovered(true);
            else m_view_model.setImguiHovered(false);

            m_view_model.handleKeysInput(); //обработка нажатия клавиш на каждом цикле для ускорения обработки

            render();

        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void render(){
        // using namespace std::chrono_literals;
        // auto end = std::chrono::steady_clock::now() + 16ms;
        // Сначала рендерим объекты, а затем GUI часть
        m_view_model.render();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

            fpsWidget();
            mainWidgetRender();
            cameraWidgetRender();
            planetsWidgetRender();

        ImGui::Render();
        glClearColor(0.2f, 0.2f, 0.2f, 0.5f);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // std::this_thread::sleep_until(end);
        glfwMakeContextCurrent(m_window);
        glfwSwapBuffers(m_window);
    }

    void fpsWidget(){
        ImVec2 pos = {static_cast<float>(m_view_model.getWindowWidth() - 100), 0};
        ImVec2 size = {100, 20};

        ImGui::SetNextWindowPos(pos);
        ImGui::SetNextWindowSize(size);

        ImGui::Begin("Performance", NULL,
         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);
        ImGui::Text("FPS: %.f", fps);
        ImGui::End();
    }
    void mainWidgetRender(){
        static int rings_counter = 5;
        static int sats_counter = 20;
        ImVec2 pos = m_view_model.getImguiData().main_widget_pos;
        ImVec2 size = {m_view_model.getImguiData().main_widget_size};

        ImGui::SetNextWindowPos(pos);
        ImGui::SetNextWindowSize(size);

        ImGui::Begin("Контроллер плоскостей", NULL, ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
        
        ImGui::PushItemWidth(200.0f); // Установить ширину слайдера в 200 пикселей
        if(ImGui::SliderInt("Количество плоскостей", &rings_counter, 0, 10)){
            m_view_model.updateRings(rings_counter);
            UpdateRingsItems();
        }

        if(ImGui::SliderInt("Количество спутников", &sats_counter, 0, 30) && (m_view_model.getRingsQuantity() > 0)){
            m_view_model.updateSatellites(sats_counter);
        }

        if(ImGui::Button("Движение спутников")){
            m_view_model.setSatellitesMovement(m_view_model.getSatellitesMovement() ^ true);
        }

        if(ImGui::Combo("Список плоскостей", &selected_ring_index, m_rings_items.data(), m_rings_items.size())){
            m_view_model.selectRingByDisplayName(m_rings_items[selected_ring_index]);
        }
        
        ImGui::PopItemWidth();
        m_view_model.setImguiMainWidgetSize(ImGui::GetWindowSize());
        
        ImGui::End();
    }

    void cameraWidgetRender(){
        ImVec2 pos_2 = {m_view_model.getImguiData().camera_widget_pos};
        ImVec2 size_2 = {m_view_model.getImguiData().camera_widget_size};

        ImGui::SetNextWindowPos(pos_2);
        ImGui::SetNextWindowSize(size_2);
        ImGui::Begin("Контроллер камеры", NULL, ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
        
        ImGui::PushItemWidth(200.0f); // Установить ширину слайдера в 200 пикселей
        
        if(ImGui::Combo("Список планет", &selected_planet_index, m_planets_items.data(), m_planets_items.size())){
            m_view_model.setCameraTarget(m_view_model.getPlanetCenterVec(selected_planet_index));
        }
        if(ImGui::Button("Полёт камеры")){
            m_view_model.setCameraMovement(m_view_model.getCameraMovement() ^ true);
        }
        ImGui::LabelText("Орбитальная камера", "Текущая камера:");
        auto pos = m_view_model.getCameraPosition();
        ImGui::Text("Позиция x: %.2f y: %.2f z: %.2f", pos.x, pos.y, pos.z);
        
        auto target = m_view_model.getCameraTarget();
        ImGui::Text("Таргет x: %.2f y: %.2f z: %.2f", target.x, target.y, target.z);

        ImGui::Text("Расстояние: %.2f", m_view_model.getDistance());
        ImGui::PopItemWidth();

        m_view_model.setImguiSecWidgetSize(ImGui::GetWindowSize());
        
        ImGui::End();
    }

    void planetsWidgetRender(){
        ImVec2 pos = {m_view_model.getImguiData().planets_widget_pos};
        ImVec2 size = {m_view_model.getImguiData().planets_widget_size};

        ImGui::SetNextWindowPos(pos);
        ImGui::SetNextWindowSize(size);

        ImGui::Begin("Контроллер планет", NULL, ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
        if(ImGui::Button("Вращение планет")){
            m_view_model.setPlanetsRotation(m_view_model.getPlanetsRotation() ^ true);
        }
        if(ImGui::Button("Движение планет")){
            m_view_model.setPlanetsMovement(m_view_model.getPlanetsMovement() ^ true);
        }
        ImGui::End();

    }

    void updateFPS() {
        double currentTime = glfwGetTime();
        frameCount++;
        if (currentTime - lastTime >= 1.0) { // Обновляем FPS каждую секунду
            fps = frameCount / (currentTime - lastTime);
            frameCount = 0;
            lastTime = currentTime;
        }
    }
    void updateModel(){

    }
private:
    DemoViewModel&  m_view_model;
    GLFWwindow *m_window;

    std::vector<const char*> m_planets_items = {"Солнце", "Меркурий", "Земля", "Марс"};
    int selected_planet_index = 2;

    std::vector<std::string> m_rings_names = {};
    std::vector<const char*> m_rings_items = {};
    int selected_ring_index = -1;

    double lastTime = 0.0;
    int frameCount = 0;
    float fps = 0.0f;

    void UpdateRingsItems(){
        m_rings_items.clear();
        m_rings_items.reserve(m_view_model.getRingsQuantity());

        m_rings_names = m_view_model.getRingsDisplayNames();
        for(auto& name: m_rings_names){
            m_rings_items.push_back(name.c_str());
        }
    }
};
#endif //DEMO_MVC_HPP