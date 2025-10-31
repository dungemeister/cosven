#ifndef RING_HPP
#define RING_HPP

#include "satellite.hpp"
#include <memory>

class RingSegment {
public:
    GLuint vao, vbo;
    std::vector<float> vertices;
    glm::vec3 color; // Цвет линии
    glm::vec3 offset_vec;

    RingSegment(glm::vec3 offset_vec_ = {0.f, 0.f, 0.f}){
        offset_vec = offset_vec_;
        vertices.reserve(100);
        // Настройка VAO и VBO
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
    };
    void SetParams(float radius, float ring_angle, int sats_qty, const glm::vec3& color = glm::vec3(1.0f, 0.3f, 0.3f)) {
        vertices.clear();

        this->color = color;
        float sat_angle = 360.0f / sats_qty;

        // Генерируем вершины для линий между спутниками
        for (int i = 0; i < sats_qty; i++) {
            float phi_angle = sat_angle * i;
            float y = glm::sin(glm::radians(phi_angle)) * radius + offset_vec.y;
            float x = glm::cos(glm::radians(phi_angle)) * glm::sin(glm::radians(ring_angle)) * radius + offset_vec.x;
            float z = glm::cos(glm::radians(phi_angle)) * glm::cos(glm::radians(ring_angle)) * radius + offset_vec.z;
            
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
        // Добавляем первую вершину в конец, чтобы замкнуть кольцо
        vertices.push_back(vertices[0]);
        vertices.push_back(vertices[1]);
        vertices.push_back(vertices[2]);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
        // CheckGLError("RingSegment Setup");
    }

    void Render(GLuint shaderProgram, const glm::mat4& mvp) {
        glUseProgram(shaderProgram);
        GLint mvpLoc = glGetUniformLocation(shaderProgram, "mvp");
        if (mvpLoc == -1) printf("Ошибка: униформа 'mvp' не найдена\n");
        else glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);

        GLint colorLoc = glGetUniformLocation(shaderProgram, "lineColor");
        if (colorLoc == -1) printf("Ошибка: униформа 'lineColor' не найдена\n");
        else glUniform3fv(colorLoc, 1, &color[0]);

        glBindVertexArray(vao);
        glDrawArrays(GL_LINE_STRIP, 0, vertices.size() / 3);
        glBindVertexArray(0);
        // CheckGLError("RingSegment Render");
    }

    ~RingSegment() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }
};

class Ring{
public:
    struct SatData{
        float       angle;
        glm::mat4   transform_matrix;
    };
    Ring(int ring_num, const std::string& body_texture_file, const std::string& wing_texture_file, glm::vec3 offset_vec = glm::vec3(0.f, 0.f, 0.f)):
        m_ring_num(ring_num),
        m_radius(10),
        m_offset_vector(offset_vec),
        m_selected(false)
    {
        m_display_name = "Плоскость " + std::to_string(ring_num + 1);
        rings_qty++;
        updateRingsAngle();
        m_satellites_matrix.reserve(100);
        m_satellites_model = std::make_shared<Satellite>(body_texture_file, wing_texture_file);
        m_segments = std::make_shared<RingSegment>(offset_vec);

    }
    Ring(const Ring& other):
        m_radius(other.m_radius),
        m_ring_angle(other.m_ring_angle),
        m_ring_num(other.m_ring_num),
        m_offset_vector(other.m_offset_vector),
        m_sat_angle(other.m_sat_angle),
        m_satellites_matrix(other.m_satellites_matrix),
        m_satellites_model(other.m_satellites_model ? std::make_shared<Satellite>(*other.m_satellites_model) : nullptr),
        m_segments(other.m_segments ? std::make_shared<RingSegment>(*other.m_segments) : nullptr),
        m_display_name(other.m_display_name),
        m_selected(other.m_selected)
    {
        rings_qty++;
    }
    Ring(Ring&& other) noexcept:
        m_radius(other.m_radius),
        m_ring_angle(other.m_ring_angle),
        m_ring_num(other.m_ring_num),
        m_offset_vector(other.m_offset_vector),
        m_sat_angle(other.m_sat_angle),
        m_satellites_matrix(other.m_satellites_matrix),
        m_satellites_model(std::move(other.m_satellites_model)),
        m_segments(std::move(other.m_segments)),
        m_display_name(other.m_display_name),
        m_selected(other.m_selected)
    {
        rings_qty++;

    }
    ~Ring(){
        rings_qty--;
    }

    void pushSatellites(int sats_qty);
    void pushSatellite();
    void popSatellite();
    void popSatellites(int sats_qty);
    void clear();

    int getSatellitesQty() { return m_satellites_matrix.size(); }

    void rotateRing(float d_angle);
    void shiftRingAngle(float delta_angle);
    void render(GLuint shaderProgram, GLuint segmentShaderProgram, const glm::mat4& view,
                 const glm::mat4& projection, GLuint borderShaderProgram = 0);

    void updateRingsAngle() { m_ring_angle = 180.0f / rings_qty * (m_ring_num + 1); }
    void setOffsetVector(const glm::vec3& vector) { m_offset_vector = vector; }

    std::string getDisplayName() { return m_display_name; }
    void setSelected(bool selected) { 
        m_selected = selected;
        m_satellites_model->SetSelected(selected);
    }

private:
    static int rings_qty;
    const int m_segments_qty = 30;
    std::vector<SatData> m_satellites_matrix;
    std::shared_ptr<Satellite> m_satellites_model;
    std::shared_ptr<RingSegment> m_segments;
    int m_ring_num;
    float m_radius;
    float m_ring_angle;
    float m_sat_angle;
    glm::vec3 m_offset_vector;

    std::string m_display_name;
    bool m_selected;
    glm::mat4 MoveToPosition(const glm::mat4& currentTransform, const glm::vec3& target);
};



#endif //RING_HPP