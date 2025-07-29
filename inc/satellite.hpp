#ifndef SATELLITE_HPP
#define SATELLITE_HPP
#include <string>
#include "shaderHandler.hpp"
#include "model.hpp"

class Satellite{

public:
    Satellite(const std::string& body_texture_file, const std::string& wing_texture_file);
    ~Satellite() {
        delete body;
        delete wings;
        // delete antenna;
    }
    void AddInstance(const glm::mat4& transform);
    void RemoveInstance();
    int  GetInstancesQty() { return m_instances_matrix.size(); }
    void RemoveAllInstances();

    void Render(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection);
private:
    Model* body;
    Model* wings;
    // Model* antenna;
    std::vector<glm::mat4> m_instances_matrix;
};
#endif //SATELLITE_HPP