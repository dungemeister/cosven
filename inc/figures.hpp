#ifndef FIGURES_HPP
#define FIGURES_HPP

#include "iostream"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "shaderHandler.hpp"



class Torus
{
public:
    Torus(GLFWwindow *window, float ext_radius, float internal_radius, int ext_step, int int_step,
     bool texture);
    ~Torus();

    void createModel();
    void createModelWithTexture();
    bool loadShaderSource(std::string filepath, shaderType type);
    bool compileShader();
    void loadShaderTexture(std::string filepath, bool alpha_channel);
    void useShaderProgram();
    void render(glm::mat4 projection, glm::mat4 view);

    void loadShaderUniformInt(std::string uniform_name, GLint value);
    void loadShaderUniformFloat(std::string uniform_name, GLfloat value);

    void loadShaderProjectionMatrix(glm::mat4 projection);
    void loadShaderModelMatrix(glm::mat4 model);
    void loadShaderViewMatrix(glm::mat4 view);
    void loadShaderUniformVec3(std::string uniform_name, glm::vec3 vec);
    void rotate(float degree, glm::vec3 axis_vec);

    void setPolygonMode(bool polygon) {m_polygon_mode = polygon;};
    bool getPolygonMode(){return m_polygon_mode;};
private:

    GLFWwindow *m_window = nullptr;
    ShaderHandler *m_program = nullptr;
    bool m_texture = false;

    GLfloat m_ext_radius;
    GLfloat m_int_radius;
    int m_ext_step;
    int m_int_step;

    GLuint m_external_qty = 0;
    GLuint m_internal_qty = 0;
    GLuint m_vertices_qty = 0;
    GLuint m_indices_qty = 0;

    struct vertice{
        GLfloat x,y,z;
    } *m_vertices = nullptr;

    GLuint *m_indices = nullptr;

    glm::mat4 m_model = glm::mat4(1.0f);
    GLuint m_vao = {};
    GLuint m_vbo = {};
    GLuint m_ebo = {};

    bool m_polygon_mode = true;
};
#endif //FIGURES_HPP