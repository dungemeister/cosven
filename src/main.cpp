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

using namespace std;

#define OBJECTS_QTY (3)

#define numVAOs (OBJECTS_QTY)
#define numVBOs (OBJECTS_QTY)
#define numEBOs (OBJECTS_QTY)

#define LEFT_WING_INDEX (0)
#define RIGHT_WING_INDEX (1)
#define MAIN_BODY_INDEX (1)

bool polygon_fill = true;

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint ebo[numEBOs];

bool mouse_rotation = false;
double g_xpos, g_ypos;

uint32_t g_width = 800;
uint32_t g_height = 600;

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

    if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        polygon_fill ^= true;
        cout << (polygon_fill ? "true": "false") << endl;

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
    // cout << width << " " << height <<endl;
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
    float z = 0;
    float radius = 10.0f;
    int sat_qty = 5;
    for(int i = 0; i < sat_qty; i++)
    {
        Satellite *sat = new Satellite(window, "satellite " + std::to_string(i));
        if(!sat->loadShaderSource("/home/yura/opengl/src/vshader_wings.glsl", vertex_shader))
            return -1;
        if(!sat->loadShaderSource("/home/yura/opengl/src/fshader_wings.glsl", fragment_shader))
            return -2;
        if(sat->compileShader() != shaderCompileOk)
            return -3;

        sat->loadShaderTexture("/home/yura/opengl/solar_battery.jpg", false);
        sat->loadShaderTexture("/home/yura/opengl/body_texture.jpg", false);
        // sat->loadShaderTexture("/home/yura/opengl/wall_texture.jpg", false);
        
        sat->useShaderProgram();
        sat->createModel();
        
        z = glm::sin(glm::radians(i * (float)360/(float)sat_qty)) * radius;
        x = glm::cos(glm::radians(i * (float)360/(float)sat_qty)) * radius;

        cout << "x " << x << " z " << z << endl;
        sat->translateModel(glm::vec3(x, 0.0f, -z));
        sat->rotateModel(10.0f * (i+ 1), glm::vec3(2.0f * (i + 1), 0.0f, 0.0f));
        
        sat->loadShaderViewMatrix(view);

        sats.push_back(sat);
    }

        
    glClearColor(1.f, 0.2f, 1.0f, 0.5f);

    auto time = glfwGetTime();
    int i = 0;
    while(!glfwWindowShouldClose(window)){
        using namespace std::chrono_literals;
        
        processInput(window);

        auto end = std::chrono::steady_clock::now() + 1ms;
        time = glfwGetTime();
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        i = 0;
        
        for(auto sat: sats)
        {
            
            sat->useShaderProgram();
            sat->setPolygonMode(polygon_fill);
            projection = glm::perspective(glm::radians(camera.Zoom), (float)g_width/(float)g_height, 0.1f, 100.0f);
            sat->loadShaderUniformFloat("u_green", glm::sin(glm::radians(50 * time)));
            sat->loadShaderProjectionMatrix(projection);
            sat->loadShaderViewMatrix(camera.GetViewMatrix());
            sat->rotateModel(-0.5 * (i + 1), glm::vec3(0.0f, 1.0f, 0.0f));
            sat->renderModel();

            if(printOpenGLError())
            {
                glfwDestroyWindow(window);
                glfwTerminate();
                return -10;
            }
            i++;
        }

        std::this_thread::sleep_until(end);
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(numVAOs, &vao[0]);
    glDeleteBuffers(numVBOs, &vbo[0]);
    glDeleteBuffers(numVBOs, &ebo[0]);

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
    std::cout << "Drawing...\n";

    int res = satellites_class_render(window);
    return res;
    
}
