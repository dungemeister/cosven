#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>


using namespace std;

#define numVAOs 2
#define numVBOs 2

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];


struct shaderCfg{
    GLuint shader = {};
    string filename = {};
    string sourceFile = {};
    bool error = false;
    string error_log = {};

    void printError()
    {
        cout << "Shader " << filename << " error: " << error_log << endl;
    }
};

shaderCfg vShader = {};
shaderCfg fShader = {};

string readShaderSourceFromFile(string filePath)
{
    string content;

    ifstream fileStream(filePath, ios::in);
    string line = "";
    if (!fileStream.is_open())
        return "";
    while(!fileStream.eof())
    {
        getline(fileStream, line);
        content.append(line);
        content.append("\n");
    }
    fileStream.close();
    return content;
}

void getShaderLog(shaderCfg* shaderCfg)
{
    int len = 0;
    int chWritten = 0;
    char *log;
    glGetShaderiv(shaderCfg->shader, GL_INFO_LOG_LENGTH, &len);
    if(len > 0)
    {
        log = (char *)malloc(len);
        glGetShaderInfoLog(shaderCfg->shader, len, &chWritten, log);
        // cout << "Shader log: " << log << endl;
        shaderCfg->error = true;
        shaderCfg->error_log = string(log);
        free(log);
    }
}

bool printOpenGLError()
{
    bool found_error = false;
    int glErr = glGetError();
    while(glErr != GL_NO_ERROR)
    {
        cout << "glError: " << glErr << endl;
        found_error = true;
        glErr = glGetError();
    }
    return found_error;
}

GLuint createShaderProgram()
{
    vShader.filename = string("/home/yura/opengl/src/vshader.glsl");
    fShader.filename = string("/home/yura/opengl/src/fshader.glsl");

    vShader.sourceFile = readShaderSourceFromFile(vShader.filename);
    fShader.sourceFile = readShaderSourceFromFile(fShader.filename);

    const char * vshaderChars = vShader.sourceFile .c_str();
    const char * fshaderChars = fShader.sourceFile.c_str();

    vShader.shader = glCreateShader(GL_VERTEX_SHADER);
    fShader.shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vShader.shader, 1, &vshaderChars, NULL);
    glShaderSource(fShader.shader, 1, &fshaderChars, NULL);

    glCompileShader(vShader.shader);
    glCompileShader(fShader.shader);

    getShaderLog(&vShader);
    getShaderLog(&fShader);

    if(vShader.error)
    {
        vShader.printError();
        return 0;
    }

    if(fShader.error)
    {
        fShader.printError();
        return 0;
    }

    GLuint vfProgram = glCreateProgram();
    glAttachShader(vfProgram, vShader.shader);
    glAttachShader(vfProgram, fShader.shader);
    glLinkProgram(vfProgram);

    printOpenGLError();
    return vfProgram;
    
}

void display(GLFWwindow *window , double time)
{
    glUseProgram(renderingProgram);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(vao[0]);
    glPointSize(10.0f);
    glDrawArrays(GL_POINTS, 0, 1);

    glBindVertexArray(vao[1]);
    glPointSize(10.0f);
    glDrawArrays(GL_POINTS, 0, 1);

    printOpenGLError();
}

void arrays_init()
{
    GLfloat *vert = {};
    glGenVertexArrays(numVAOs, &vao[0]);

    delete vert;

}

void default_init()
{
    renderingProgram = createShaderProgram();
    glGenVertexArrays(numVAOs, &vao[0]);
    glBindVertexArray(vao[0]);
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
    
    GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL tutorials", NULL, NULL);

    glfwMakeContextCurrent(window);
    if(glewInit() != GLEW_OK){
        return -2;
    }
    glfwSwapInterval(1);

    glClearColor(0.0, 0.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    arrays_init();
    // default_init();

    std::cout << "Drawing...\n";

    while(!glfwWindowShouldClose(window)){
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
