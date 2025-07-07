#include "ring.hpp"

void Ring::pushSatellite(){
    m_sats_qty++;
}

void Ring::pushSatellites(int sats_qty){
    clear();

    m_sat_angle = 360.0f / sats_qty;
    for(int i = 0; i < sats_qty; i++){
        auto phi_angle = m_sat_angle * i;
        float y = glm::sin(glm::radians(phi_angle)) * m_radius;
        float x = glm::cos(glm::radians(phi_angle)) * glm::sin(glm::radians(m_ring_angle)) * m_radius;
        float z = glm::cos(glm::radians(phi_angle)) * glm::cos(glm::radians(m_ring_angle)) * m_radius;
        
        m_satellites_model->AddInstance(glm::vec3(x, y, -z));
        m_sats_qty++;
    }

}

void Ring::popSatellite(){
    if(m_sats_qty <= 0) return;
    m_satellites_model->RemoveInstance();
    m_sats_qty--;
}

void Ring::popSatellites(int sats_qty){
    for(int i = 0; i < sats_qty; i++)
        popSatellite();
}
void Ring::clear(){
    m_satellites_model->RemoveAllInstances();
    m_sats_qty = 0;
}

void Ring::rotateRing(float angle){
    m_satellites_model->RemoveAllInstances();

    for(int i = 0; i < m_sats_qty; i++){
        auto phi_angle = m_sat_angle * (i + 1) + angle;
        float y = glm::sin(glm::radians(phi_angle)) * m_radius;
        float x = glm::cos(glm::radians(phi_angle)) * glm::sin(glm::radians(m_ring_angle)) * m_radius;
        float z = glm::cos(glm::radians(phi_angle)) * glm::cos(glm::radians(m_ring_angle)) * m_radius;
        
        m_satellites_model->AddInstance(glm::vec3(x, y, -z));
    }
    m_sat_angle += angle;

}

void Ring::render(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection){
    m_satellites_model->Render(shaderProgram, view, projection);
}