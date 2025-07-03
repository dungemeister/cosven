#ifndef SATELLITE_HPP
#define SATELLITE_HPP
#include <string>
#include "shaderHandler.hpp"
#include "satellite_model.hpp"

class Satellite{

public:
    Satellite(GLFWwindow *window, std::string name);
    ~Satellite();
    bool loadShaderSource(std::string filepath, shaderType type);
    bool compileShader();
    void loadShaderTexture(std::string filepath, bool alpha_channel);
    void useShaderProgram();
    void createModel();
    void renderModel();

    void loadShaderUniformInt(std::string uniform_name, GLuint value);
    void loadShaderUniformFloat(std::string uniform_name, GLfloat value);

    void loadShaderProjectionMatrix(glm::mat4 projection);
    void loadShaderModelMatrix(glm::mat4 model);
    void loadShaderViewMatrix(glm::mat4 view);

    void translateModel(glm::vec3 translate_vec);
    void rotateModel(float degree, glm::vec3 translate_vec);
    void scaleModel(glm::vec3 translate_vec);
    glm::mat4 getModelMatrix() { return m_model->getModelMatrix(); }
    
    void setPolygonMode(bool fill_mode);

    void setAlphaAngle(float alpha) { m_alpha_angle = alpha; }
    float getAlphaAngle() { return m_alpha_angle; }

    void setPhiAngle(float phi) { m_phi_angle = phi; }
    float getPhiAngle() { return m_phi_angle; }

    void setSatelliteNum(int satellite_num) { m_satellite_num = satellite_num; }
    int getSatelliteNum() { return m_satellite_num; }

    void setRingNum(int ring_num) { m_ring_num = ring_num; }
    int getRingNum() { return m_ring_num; }

    void setRadius(float radius) { m_radius = radius; }
    float getRadius() { return m_radius; }
private:
    std::string m_name;
    ShaderHandler *m_program;
    SatelliteModel *m_model;
    ModelPolygonMode m_fill = POLYGON_MODE_FILL;
    GLFWwindow *m_window;
    float m_alpha_angle = 0.0f;
    float m_phi_angle = 0.0f;
    int m_satellite_num = 0;
    int m_ring_num = 0;
    float m_radius;

    GLuint m_vao[VAO_ARRAYS_QTY] = {};
    GLuint m_vbo[VAO_ARRAYS_QTY] = {};
    GLuint m_ebo[VAO_ARRAYS_QTY] = {};
};
#endif //SATELLITE_HPP