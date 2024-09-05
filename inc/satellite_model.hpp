#ifndef SATELLITE_MODEL_HPP
#define SATELLITE_MODEL_HPP

#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define VAO_ARRAYS_QTY (2)



enum ModelPolygonMode{
    POLYGON_MODE_FILL = 0,
    POLYGON_MODE_LINE,
    POLYGON_MODE_POINT,
};

class SatelliteModel
{
public:
    SatelliteModel(GLFWwindow *window, std::string name);
    ~SatelliteModel();
    void createModel(GLuint *vao, GLuint *vbo, GLuint *ebo);
    void renderWings(GLuint buf_index);
    void renderBody(GLuint buf_index);
    
    glm::mat4 getModelMatrix();
    glm::mat4 rotateModel(float degrees, glm::vec3 axis_vec);
    glm::mat4 translateModel(glm::vec3 axis_vec);
    glm::mat4 scaleModel(glm::vec3 translate_vec3);

    GLint getWingsVao() { return m_vao[m_wings_array_index];}
    const uint32_t getWingsElementsQty() {return m_wings_elements_qty;}
    const uint32_t getBodyElementsQty() {return m_body_elements_qty;}

private:
    std::string m_name;
    uint32_t m_sat_num;
    uint32_t m_ring_num;

    const uint32_t m_wings_array_index = 0;
    const uint32_t m_body_array_index = 1;

    GLuint m_vao[VAO_ARRAYS_QTY] = {};
    GLuint m_vbo[VAO_ARRAYS_QTY] = {};
    GLuint m_ebo[VAO_ARRAYS_QTY] = {};

    uint32_t m_wings_elements_qty;
    uint32_t m_body_elements_qty;
    glm::mat4 m_model = glm::mat4(1.0f);

};

#endif //SATELLITE_MODEL_HPP