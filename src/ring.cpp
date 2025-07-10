#include "ring.hpp"

void Ring::pushSatellite(){
}

void Ring::pushSatellites(int sats_qty){
    clear();

    m_sat_angle = 360.0f / sats_qty;

    m_segments->SetParams(m_radius, m_ring_angle, sats_qty);
    for(int i = 0; i < sats_qty; i++){
        auto phi_angle = m_sat_angle * (i + 1);
        float y = glm::sin(glm::radians(phi_angle)) * m_radius;
        float x = glm::cos(glm::radians(phi_angle)) * glm::sin(glm::radians(m_ring_angle)) * m_radius;
        float z = glm::cos(glm::radians(phi_angle)) * glm::cos(glm::radians(m_ring_angle)) * m_radius;
        
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
        m_satellites_model->AddInstance(transform);
        m_satellites_matrix.push_back({phi_angle, transform});
    }

}

void Ring::popSatellite(){
    if(m_satellites_matrix.size() <= 0) return;
    m_satellites_model->RemoveInstance();
    m_satellites_matrix.pop_back();
}

void Ring::popSatellites(int sats_qty){
    for(int i = 0; i < sats_qty; i++)
        popSatellite();
}
void Ring::clear(){
    m_satellites_model->RemoveAllInstances();
    m_satellites_matrix.clear();
}

void Ring::rotateRing(float d_angle){
    m_satellites_model->RemoveAllInstances();

    for(int i = 0; i < m_satellites_matrix.size(); i++){
        m_satellites_matrix[i].angle += d_angle;
        float y = glm::sin(glm::radians(m_satellites_matrix[i].angle)) * m_radius;
        float x = glm::cos(glm::radians(m_satellites_matrix[i].angle)) * glm::sin(glm::radians(m_ring_angle)) * m_radius;
        float z = glm::cos(glm::radians(m_satellites_matrix[i].angle)) * glm::cos(glm::radians(m_ring_angle)) * m_radius;
        
        m_satellites_matrix[i].transform_matrix = MoveToPosition(m_satellites_matrix[i].transform_matrix, glm::vec3(x, y, z)); 
        m_satellites_model->AddInstance(m_satellites_matrix[i].transform_matrix);
    }

}

void Ring::render(GLuint shaderProgram, GLuint segmentShaderProgram, const glm::mat4& view, const glm::mat4& projection){
    m_satellites_model->Render(shaderProgram, view, projection);
    if(m_segments)
        m_segments->Render(segmentShaderProgram, projection * view);
}

glm::mat4 Ring::MoveToPosition(const glm::mat4& currentTransform, const glm::vec3& target) {
    // Извлекаем текущее положение из матрицы (4-й столбец: x, y, z)
    glm::vec3 currentPos = glm::vec3(currentTransform[3]);
    // Вычисляем вектор смещения (находим разницу векторов)
    glm::vec3 offset = target - currentPos;
    // Применяем трансляцию
    return glm::translate(glm::mat4(1.0f), offset) * currentTransform;
}