#ifndef EARTH_HPP
#define EARTH_HPP

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

    NewEarth(const std::string& texture_file, float radius = 8.0f);
    ~NewEarth() = default;
    void PushEarth(const glm::vec3& coord);
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
    glm::vec3 GetCenterCoords() { return m_center_coords; }
    void Move() {
        auto radius = glm::distance(m_center_coords, {0.f, 0.f, 0.f});
        float y = m_center_coords.y;
        float x = glm::sin(glm::radians(m_current_angle)) * radius;
        float z = glm::cos(glm::radians(m_current_angle)) * radius;
        
        // auto curPos = m_transform_matrix[3];
        // std::cout << curPos.x << " " << curPos.y << " " << curPos.z << "\n";
        // m_transform_matrix = MoveToPosition(m_transform_matrix, glm::vec3(x, y, z));
        // m_transform_matrix = transform;
        m_transform_matrix = glm::translate(glm::mat4{1.f}, glm::vec3(x, y, z) + m_center_coords);
        m_current_angle += 1.f;
        m_center_coords = {x, y, z};
    }
private:
    std::vector<float> m_vertices;
    std::vector<uint> m_indices;
    float m_radius;
    glm::vec3 m_center_coords;
    float m_current_angle;
    const static GLuint m_vertical_qty = 180/VER_STEP_ANGLE + 1;
    const static GLuint m_horizontal_qty = 360/HOR_STEP_ANGLE + 1;
    const static GLuint m_vertices_qty = m_vertical_qty * m_horizontal_qty;
    const static GLuint m_indices_qty = 2 * (m_vertices_qty - m_horizontal_qty);

    glm::mat4 m_transform_matrix;
    std::shared_ptr<Model> m_earth;

    glm::mat4 MoveToPosition(const glm::mat4& currentTransform, const glm::vec3& target){
        // Извлекаем текущее положение из матрицы (4-й столбец: x, y, z)
        glm::vec3 currentPos = glm::vec3(currentTransform[3]);
        // Вычисляем вектор смещения (находим разницу векторов)
        glm::vec3 offset = target - currentPos;
        // Применяем трансляцию
        return glm::translate(glm::mat4(1.0f), offset) * currentTransform;
    }
};

#endif //EARTH_HPP