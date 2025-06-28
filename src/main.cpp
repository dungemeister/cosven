#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <thread>
#include <vector>

#include "satellite.hpp"
#include "camera.hpp"
#include "skybox.hpp"
#include "earth.hpp"
#include "figures.hpp"

using namespace std;

#define OBJECTS_QTY (3)

#define numVAOs (OBJECTS_QTY)
#define numVBOs (OBJECTS_QTY)
#define numEBOs (OBJECTS_QTY)

#define LEFT_WING_INDEX (0)
#define RIGHT_WING_INDEX (1)
#define MAIN_BODY_INDEX (1)

bool polygon_fill = true;
bool satellites_movement = false;
float satellite_rotate_angle = 0.1f;

GLuint renderingProgram;

bool mouse_rotation = false;
double g_xpos, g_ypos;
int SAT_INDEX = -1;
uint32_t g_width = 1200;
uint32_t g_height = 1000;

const glm::vec3 default_camera_position = glm::vec3(5.0f, 0.0f, 10.0f);
Camera camera(default_camera_position);
float lastX = g_width / 2.0f;
float lastY = g_height / 2.0f;
bool firstMouse = true;

float deltatime = 0.08;
float lastFrame = 0;

glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);

struct cameraParams{
    float x;
    float y;
    float z;
};

glm::mat4 transform = glm::mat4(1.0f);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        polygon_fill ^= true;
        cout << "polygons " << (polygon_fill ? "true": "false") << endl;
        if(polygon_fill)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    }
    if(glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
        satellites_movement ^= true;
        cout << "movement " << (satellites_movement ? "true": "false") << endl;

    }
    if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        SAT_INDEX = 1;
    }
    if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        SAT_INDEX = 2;
    }
    if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        SAT_INDEX = 3;
    }
    if(glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        SAT_INDEX = 4;
    }
    if(glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
    {
        SAT_INDEX++;
    }
    if(glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
    {
        SAT_INDEX--;
    }
    if(glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    {
        satellite_rotate_angle += 0.1f;

    }
    if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        satellite_rotate_angle -= 0.1f;
    }
    return;
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        camera = Camera(default_camera_position);
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

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float> (yoffset));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
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

void mouse_cursor_callback(GLFWwindow *window, double xpos, double ypos)
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
    camera.ProcessMouseMovement(xoffset, yoffset);

}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    // projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
    g_width = width;
    g_height = height;

}

string getHumanOpenGLErrorEnum(int glErr)
{
    switch(glErr)
    {
        case 0:
            return "no error";
        case 1280:
            return "invalid enum";
        case 1281:
            return "invalid value";
        case 1282:
            return "invalid operation";
        case 1283:
            return "stack overflow";
        case 1284:
            return "stack underflow";
        case 1285:
            return "out of memory";
        case 1286:
            return "invalid framebuffer operation";
        default:
            return "unknown error enum";
    }
}

bool printOpenGLError()
{
    bool found_error = false;
    int glErr = glGetError();

    while(glErr != GL_NO_ERROR)
    {
        cout << "glError: " << getHumanOpenGLErrorEnum(glErr) << endl;
        found_error = true;
        glErr = glGetError();
    }
    return found_error;
}

int satellites_class_render(GLFWwindow *window)
{
    glEnable(GL_DEPTH_TEST);
    
    projection = glm::perspective(glm::radians(45.0f), (float)g_width / (float)g_height, 0.1f, 100.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    std::vector<Satellite*> sats;
    float x = 0;
    float y = 0;
    float z = 0;
    float radius = 10.0f;
    int sat_qty = 5;
    int rings = 1;
    for(int ring = 0; ring < rings; ring++)
    {
        float alpha = 10 + (ring + 0) * (float)(180)/(float)rings;

        for(int i = 0; i < sat_qty; i++)
        {
            Satellite *sat = new Satellite(window, "satellite " + std::to_string(i));
            sat->setRingNum(ring);
            sat->setSatelliteNum(i);

            if(!sat->loadShaderSource("../shaders/vshader_satellite.glsl", vertex_shader))
                return -1;
            if(!sat->loadShaderSource("../shaders/fshader_satellite.glsl", fragment_shader))
                return -2;
            if(sat->compileShader() != shaderCompileOk)
                return -3;
            sat->loadShaderTexture("../textures/solar_battery.jpg", false);
            sat->loadShaderTexture("../textures/body_texture.jpg", false);
            // sat->loadShaderTexture("/home/yura/opengl/wall_texture.jpg", false);
            
            sat->useShaderProgram();
            sat->createModel();
            
            float phi = (2 * ring) + i * (float)(360)/(float)sat_qty;

            y = glm::sin(glm::radians(phi)) * radius;
            x = glm::cos(glm::radians(phi)) * glm::sin(glm::radians(alpha)) * radius;
            z = glm::cos(glm::radians(phi)) * glm::cos(glm::radians(alpha)) * radius;

            cout << "x " << x << " y " << y << " z " << z << endl;
            sat->translateModel(glm::vec3(x, y, -z));
            // sat->rotateModel(10.0f * (i+ 1), glm::vec3(2.0f * (i + 1), 0.0f, 0.0f));
            
            sat->setAlphaAngle(alpha);
            sat->setPhiAngle(phi);
            sats.push_back(sat);
        }
    }
        
    glClearColor(0.2f, 0.2f, 0.2f, 0.5f);

    Skybox *skybox = new Skybox(window, "skybox");
        if(!skybox->loadShaderSource("../shaders/skybox_vshader.glsl", vertex_shader))
            return -1;
        if(!skybox->loadShaderSource("../shaders/skybox_fshader.glsl", fragment_shader))
            return -2;
        if(skybox->compileShader() != shaderCompileOk)
            return -3;
    skybox->loadSkyboxCubemap({
        "../test_skybox/right.jpg",
        "../test_skybox/left.jpg",
        "../test_skybox/top.jpg",
        "../test_skybox/bottom.jpg",
        "../test_skybox/front.jpg",
        "../test_skybox/back.jpg",
        });

    auto time = glfwGetTime();
    int i = 0;
    cout << rings * sat_qty << " satellites rdy" << std::endl;
    
    Earth *earth = new Earth(window, 8.0f);
    earth->createModel();
    if(!earth->loadShaderSource("../shaders/vshader_earth.glsl", vertex_shader))
        return -1;
    if(!earth->loadShaderSource("../shaders/fshader_earth.glsl", fragment_shader))
        return -2;
    if(earth->compileShader() != shaderCompileOk)
        return -3;

    glm::vec3 earth_color = glm::vec3(0.2f, 0.8f, 0.4f);

    earth->loadShaderTexture("../textures/earth.jpg", false);
    earth->useShaderProgram();
    earth->loadShaderUniformInt("u_texture", 0);

    cout << "Drawing...\n";
    int k = 0;
    while(!glfwWindowShouldClose(window)){
        using namespace std::chrono_literals;
        
        processInput(window);

        auto end = std::chrono::steady_clock::now() + 16ms;
        time = glfwGetTime();
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        i = 0;

        for(auto sat: sats)
        {
            sat->useShaderProgram();
            sat->setPolygonMode(polygon_fill);
            projection = glm::perspective(glm::radians(camera.Zoom), (float)g_width/(float)g_height, 0.1f, 100.0f);
            sat->loadShaderProjectionMatrix(projection);
            sat->loadShaderViewMatrix(camera.GetViewMatrix());
            if(satellites_movement && i == SAT_INDEX || satellites_movement && SAT_INDEX < 0)
            {

                float cur_phi = sat->getPhiAngle();
                float new_phi = cur_phi + satellite_rotate_angle;
                sat->setPhiAngle(new_phi);
                float delta_alpha = sat->getAlphaAngle();
                float r = radius;
                glm::vec4 cur_vec;
                cur_vec.y = glm::sin(glm::radians(cur_phi)) * r;
                cur_vec.x = glm::cos(glm::radians(cur_phi)) * glm::sin(glm::radians(delta_alpha)) * r;
                cur_vec.z = glm::cos(glm::radians(cur_phi)) * glm::cos(glm::radians(delta_alpha)) * r;
                cur_vec.w = 1;

                glm::vec4 new_vec;
                new_vec.y = glm::sin(glm::radians(new_phi)) * r;
                new_vec.x = glm::cos(glm::radians(new_phi)) * glm::sin(glm::radians(delta_alpha)) * r;
                new_vec.z = glm::cos(glm::radians(new_phi)) * glm::cos(glm::radians(delta_alpha)) * r;
                new_vec.w = 1;

                glm::vec4 result = new_vec - cur_vec;
                sat->translateModel(glm::vec3(result.x, result.y, -result.z));
            }
            sat->renderModel();

            if(printOpenGLError())
            {
                glfwDestroyWindow(window);
                glfwTerminate();
                // return -10;
            }
            i++;
            k++;

        }
        
        earth->useShaderProgram();
        
        earth->loadShaderProjectionMatrix(projection);
        earth->loadShaderViewMatrix(camera.GetViewMatrix());
        if(satellites_movement)
            earth->rotate(0.2f, glm::vec3(0.0f, 1.0f, 0.0f));
        earth->render();

        if(printOpenGLError())
        {
            glfwDestroyWindow(window);
            glfwTerminate();
            // return -11;
        }

        glDepthFunc(GL_LEQUAL);
        
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        // skybox->render(projection, view);
        if(printOpenGLError())
        {
            glfwDestroyWindow(window);
            glfwTerminate();
            // return -12;
        }

        std::this_thread::sleep_until(end);
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

int testing(GLFWwindow *window)
{
    projection = glm::perspective(glm::radians(45.0f), (float)g_width / (float)g_height, 0.1f, 100.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    glClearColor(0.2f, 0.2f, 0.2f, 0.5f);
    
    Skybox *skybox = new Skybox(window, "skybox");
    if(!skybox->loadShaderSource("../shaders/skybox_vshader.glsl", vertex_shader))
        return -1;
    if(!skybox->loadShaderSource("../shaders/skybox_fshader.glsl", fragment_shader))
        return -2;
    if(skybox->compileShader() != shaderCompileOk)
        return -3;
    
    skybox->loadSkyboxCubemap({
        "../../test_skybox/right.jpg",
        "../../test_skybox/left.jpg",
        "../../test_skybox/top.jpg",
        "../../test_skybox/bottom.jpg",
        "../../test_skybox/front.jpg",
        "../../test_skybox/back.jpg",
        });

    auto time = glfwGetTime();

    int i = 0;
    
    Torus *torus = new Torus(window, 8.0f, 4.0f, 9, 9, false);
    if(!torus->loadShaderSource("../shaders/vshader_torus.glsl", vertex_shader))
        return -1;
    if(!torus->loadShaderSource("../shaders/fshader_torus.glsl", fragment_shader))
        return -2;
    if(torus->compileShader() != shaderCompileOk)
        return -3;
    float start_time = glfwGetTime();
    int frame_counter = 0;
    while(!glfwWindowShouldClose(window)){
        using namespace std::chrono_literals;
        
        processInput(window);

        auto end = std::chrono::steady_clock::now() + 16ms;
        if(time - start_time > 1.0f)
        {
            std::cout << "fps " << frame_counter << std::endl;
            frame_counter = 0;
            start_time = glfwGetTime();
        }
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)g_width/(float)g_height, 0.1f, 100.0f);

        torus->rotate(0.5f, glm::vec3(0.0f, 0.0f, 1.0f));
        torus->setPolygonMode(polygon_fill);
        torus->render(projection, camera.GetViewMatrix());
        
        skybox->render(projection, camera.GetViewMatrix());
        if(printOpenGLError())
        {
            glfwDestroyWindow(window);
            glfwTerminate();
            return -10;
        }
        glfwSwapBuffers(window);
        frame_counter++;
        time = glfwGetTime();
        std::this_thread::sleep_until(end);
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

int main(int, char**){
    std::cout << "Hello, from opengl!\n";
    if(!glfwInit()){
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    auto monitor = glfwGetPrimaryMonitor();
    auto mode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    
    GLFWwindow *window = glfwCreateWindow(g_width, g_height, "OpenGL tutorials", NULL, NULL);

    glfwMakeContextCurrent(window);
    if(glewInit() != GLEW_OK){
        return -2;
    }
    // glewExperimental = GL_TRUE;
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_cursor_callback);

    glfwSetWindowSizeCallback(window, framebuffer_size_callback);

    int res = satellites_class_render(window);
    // int res = testing(window);
    return res;
    
}
