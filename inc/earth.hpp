#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include <memory>
#include "shaderHandler.hpp"
#include "model.hpp"

#define HOR_STEP_ANGLE  (15)
#define VER_STEP_ANGLE  (9)

class NewEarth{
public:
    struct VaoCoords{
        glm::vec3 coords;
        glm::vec2 texture;
    };

    NewEarth(const std::string& texture_file);
    ~NewEarth() = default;
    void PushEarth(const glm::mat4& transform);
    void PopEarth();
    void Rotate(float degree, glm::vec3 axis_vec){
        m_transform_matrix = glm::rotate(m_transform_matrix, glm::radians(degree), axis_vec);
    }
    void Render(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection){
        if(m_earth.get()){
            glm::mat4 mvp = projection * view;
            m_earth->Render(shaderProgram, mvp, m_transform_matrix);

        }
    }
private:
    std::vector<float> m_vertices;
    std::vector<uint> m_indices;
    float m_radius;
    const static GLuint m_vertical_qty = 180/VER_STEP_ANGLE + 1;
    const static GLuint m_horizontal_qty = 360/HOR_STEP_ANGLE + 1;
    const static GLuint m_vertices_qty = m_vertical_qty * m_horizontal_qty;
    const static GLuint m_indices_qty = 2 * (m_vertices_qty - m_horizontal_qty);

    glm::mat4 m_transform_matrix;
    std::shared_ptr<Model> m_earth;    
};