#ifndef RING_HPP
#define RING_HPP

#include "satellite.hpp"
#include <memory>

class Ring{
public:
    struct SatData{
        float       angle;
        glm::mat4   transform_matrix;
    };
    Ring(int ring_num, const std::string& body_texture_file, const std::string& wing_texture_file):
    m_ring_num(ring_num),
    m_radius(10)
    {
        m_ring_angle = 180.0f / 5 * (m_ring_num + 1);
        m_satellites_matrix.reserve(100);
        m_satellites_model = std::make_shared<Satellite>(body_texture_file, wing_texture_file);

    }
    ~Ring() = default;

    void pushSatellites(int sats_qty);
    void pushSatellite();
    void popSatellite();
    void popSatellites(int sats_qty);
    void clear();

    int getSatellitesQty() { return m_satellites_matrix.size(); }

    void rotateRing(float d_angle);
    void shiftRingAngle(float delta_angle);
    void render(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection);
private:
    std::vector<SatData> m_satellites_matrix;
    std::shared_ptr<Satellite> m_satellites_model;
    int m_ring_num;
    float m_radius;
    float m_ring_angle;
    float m_sat_angle;

    glm::mat4 MoveToPosition(const glm::mat4& currentTransform, const glm::vec3& target);
};

#endif //RING_HPP