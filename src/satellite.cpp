#include "satellite.hpp"

Satellite::Satellite(const std::string& body_texture_file, const std::string& wing_texture_file){

    std::vector<GLfloat> body_whole_vertices = {
        -0.2f,  0.0f, 0.2f,  0.0f, 0.0f, //0
        -0.2f,  1.0f, 0.2f,  0.0f, 1.0f, //1
        0.2f,  1.0f, 0.2f,  1.0f, 1.0f,  //2
        0.2f,  0.0f, 0.2f,  1.0f, 0.0f,  //3

        -0.2f,  0.0f, -0.2f,  0.0f, 0.0f,//4
        -0.2f,  1.0f, -0.2f,  0.0f, 1.0f,//5
        0.2f,  1.0f, -0.2f,  1.0f, 1.0f, //6
        0.2f,  0.0f, -0.2f,  1.0f, 0.0f, //7

        -0.2f,  0.0f, 0.2f,  1.0f, 0.0f, //8
        -0.2f,  1.0f, 0.2f,  1.0f, 1.0f, //9
        -0.2f,  1.0f, -0.2f,  0.0f, 1.0f,//10
        -0.2f,  0.0f, -0.2f,  0.0f, 0.0f,//11

        0.2f,  0.0f, 0.2f,  0.0f, 0.0f,  //12
        0.2f,  1.0f, 0.2f,  0.0f, 1.0f,  //13
        0.2f,  1.0f, -0.2f,  1.0f, 1.0f, //14
        0.2f,  0.0f, -0.2f,  1.0f, 0.0f, //15

        -0.2f,  0.0f, 0.2f,  0.0f, 0.0f, //16
        0.2f,  0.0f, 0.2f,  1.0f, 0.0f,  //17
        0.2f,  0.0f, -0.2f,  1.0f, 1.0f, //18
        -0.2f,  0.0f, -0.2f,  0.0f, 1.0f,//19

        -0.2f,  1.0f, 0.2f,  0.0f, 1.0f, //20
        0.2f,  1.0f, 0.2f,  1.0f, 1.0f,  //21
        0.2f,  1.0f, -0.2f,  1.0f, 0.0f, //22
        -0.2f,  1.0f, -0.2f,  0.0f, 0.0f,//23
    };

    std::vector<GLuint> body_indices = {
        0, 1, 2,
        0, 2, 3,

        4, 5, 6,
        4, 6, 7,

        8, 9, 10,
        8, 10, 11,

        12, 13, 14,
        12, 14, 15,

        16, 17, 18,
        16, 18, 19,

        20, 21, 22,
        20, 22, 23,
    };
    body = new Model(body_whole_vertices, body_indices, body_texture_file);

    std::vector<GLfloat> wings_vertices = {
        -1.0f,  0.25f, 0.0,  0.0f, 0.0f,
        -1.0f,  0.75f, 0.0,  0.0f, 1.0f,
        -0.2f, 0.75f, 0.0,  1.0f, 1.0f,
        -0.2f, 0.25f, 0.0,  1.0f, 0.0f,
        0.2f,  0.25f, 0.0,  0.0f, 0.0f,
        0.2f,  0.75f, 0.0,  0.0f, 1.0f,
        1.0f, 0.75f, 0.0,  1.0f, 1.0f,
        1.0f, 0.25f, 0.0,  1.0f, 0.0f,
    };
    std::vector<GLuint> wings_indices = {
        0, 1, 2,
        0, 2, 3,
        4, 5, 6,
        4, 6, 7,
    };
    wings = new Model(wings_vertices, wings_indices, wing_texture_file);

    // // Антенна
    // std::vector<float> antennaVerts = {
    //     -0.1f,  0.0f, 0.0f,  0.0f, 0.0f,
    //         0.1f,  0.0f, 0.0f,  1.0f, 0.0f,
    //         0.1f,  1.0f, 0.0f,  1.0f, 1.0f,
    //     -0.1f,  1.0f, 0.0f,  0.0f, 1.0f
    // };
    // antenna = new Model(antennaVerts, wingIdx, "antenna.jpg");
}

void Satellite::AddInstance(const glm::vec3& pos) {
    body->AddInstance(pos);
    wings->AddInstance(pos);
    m_instances_pos.push_back(pos);
}

void Satellite::RemoveInstance(){
    body->RemoveInstance();
    wings->RemoveInstance();
    m_instances_pos.pop_back();
}
void Satellite::RemoveAllInstances(){
    body->RemoveAllInstances();
    wings->RemoveAllInstances();
}
void Satellite::Render(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection) {
    glm::mat4 mvp = projection * view;
    body->Render(shaderProgram, mvp, glm::mat4(1.0f));
    wings->Render(shaderProgram, mvp, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
}