#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include "shaderHandler.hpp"

#define HOR_STEP_ANGLE  (9)
#define VER_STEP_ANGLE  (9)

class Earth{
public:
    Earth(GLFWwindow *window, GLfloat radius);
    ~Earth();

    void createModel();
    bool loadShaderSource(std::string filepath, shaderType type);
    bool compileShader();
    void loadShaderTexture(std::string filepath, bool alpha_channel);
    void deleteShaderTexture();
    void useShaderProgram();
    void render();

    void loadShaderUniformInt(std::string uniform_name, GLint value);
    void loadShaderUniformFloat(std::string uniform_name, GLfloat value);

    void loadShaderProjectionMatrix(glm::mat4 projection);
    void loadShaderModelMatrix(glm::mat4 model);
    void loadShaderViewMatrix(glm::mat4 view);
    void loadShaderUniformVec3(std::string uniform_name, glm::vec3 vec);
    void rotate(float degree, glm::vec3 axis_vec);
    GLuint getTexture() { return m_program->getTextId(0); }

private:
    const static GLuint m_vertical_qty = 180/VER_STEP_ANGLE + 1;
    const static GLuint m_horizontal_qty = 360/HOR_STEP_ANGLE + 1;
    const static GLuint m_vertices_qty = m_vertical_qty * m_horizontal_qty;
    const static GLuint m_indices_qty = 2 * (m_vertices_qty - m_horizontal_qty);

    GLFWwindow *m_window = nullptr;
    ShaderHandler *m_program;

    GLfloat m_radius;
    struct {
        GLfloat x,y,z;
        GLfloat u,v;
    } m_vertices[m_vertices_qty] = {};

    GLuint m_indices[m_indices_qty] = {};

    glm::mat4 m_model = glm::mat4(1.0f);
    GLuint m_vao = {};
    GLuint m_vbo = {};
    GLuint m_ebo = {};

};