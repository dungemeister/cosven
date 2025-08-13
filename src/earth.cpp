#include "earth.hpp"

NewEarth::NewEarth(const std::string& texture_file, float radius):
    m_radius(radius),
    m_indices(m_indices_qty),
    m_transform_matrix(1.0f)
{
   const float PI = 3.14159265359f;
   auto stacks = 15;
   auto sectors = HOR_STEP_ANGLE;
    m_vertices.clear();
    m_indices.clear();

    // Генерация вершин
    for (int i = 0; i <= stacks; ++i) {
        float v = (float)i / stacks;
        float phi = v * PI;
        for (int j = 0; j <= sectors; ++j) {
            float u = (float)j / sectors;
            float theta = u * 2.0f * PI;

            float x = m_radius * sin(phi) * cos(theta);
            float y = m_radius * cos(phi);
            float z = m_radius * sin(phi) * sin(theta);

            // Позиция и текстурные координаты
            m_vertices.push_back(x);
            m_vertices.push_back(y);
            m_vertices.push_back(z);
            m_vertices.push_back(1.0f - u);
            m_vertices.push_back(v);
        }
    }
    // Генерация индексов
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < sectors; ++j) {
            int k1 = i * (sectors + 1) + j;
            int k2 = k1 + sectors + 1;

            m_indices.push_back(k1);
            m_indices.push_back(k2);
            m_indices.push_back(k1 + 1);

            m_indices.push_back(k1 + 1);
            m_indices.push_back(k2);
            m_indices.push_back(k2 + 1);
        }
    }

    m_earth = std::make_shared<Model>(m_vertices, m_indices, texture_file);
}

void NewEarth::PushEarth(const glm::vec3& coord){
    m_center_coords = coord;
    auto transform = glm::translate(glm::mat4(1.0), m_center_coords);
    m_earth->AddInstance(transform);
}

void NewEarth::PopEarth(){
    m_earth->RemoveInstance();
}