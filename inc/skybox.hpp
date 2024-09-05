#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shaderHandler.hpp"

class Skybox{
public:
    Skybox(GLFWwindow *window, std::string name);
    ~Skybox();
    void createModel();
    void createCubemapModel();
    void render();
    bool loadShaderSource(std::string filepath, shaderType type);
    bool compileShader();
    void useShaderProgram();
    void loadSkyboxCubemap(std::vector<std::string> surfaces);
    void loadUniformInt(std::string uniform_name, GLint value);
    void loadUniformMatrix4fv(std::string uniform_name, const glm::mat4 &mat);
private:
    GLuint m_vao[1] = {};
    GLuint m_vbo[1] = {};
    GLuint m_ebo[1] = {};
    GLuint m_text_id;
    ShaderHandler *m_program;
    std::vector<std::string> m_surfaces = {};
};

#endif //SKYBOX_HPP