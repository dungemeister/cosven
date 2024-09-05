#include "satellite_model.hpp"

const GLfloat wings_vertices_[] = {
        -1.0f,  0.25f, 0.0,  0.0f, 0.0f,
        -1.0f,  0.75f, 0.0,  0.0f, 1.0f,
        -0.2f, 0.75f, 0.0,  1.0f, 1.0f,
        -0.2f, 0.25f, 0.0,  1.0f, 0.0f,
        0.2f,  0.25f, 0.0,  0.0f, 0.0f,
        0.2f,  0.75f, 0.0,  0.0f, 1.0f,
        1.0f, 0.75f, 0.0,  1.0f, 1.0f,
        1.0f, 0.25f, 0.0,  1.0f, 0.0f,
};
const GLuint wings_indices_[] = {
    0, 1, 2,
    0, 2, 3,
    4, 5, 6,
    4, 6, 7,
};

const GLfloat body_vertices_[] = {
        -0.2f,  0.0f, 0.2f,  0.0f, 0.0f,
        -0.2f,  1.0f, 0.2f,  0.0f, 1.0f,
        0.2f,  1.0f, 0.2f,  1.0f, 1.0f,
        0.2f,  0.0f, 0.2f,  1.0f, 0.0f,

        -0.2f,  0.0f, -0.2f,  0.0f, 0.0f,
        -0.2f,  1.0f, -0.2f,  0.0f, 1.0f,
        0.2f,  1.0f, -0.2f,  1.0f, 1.0f,
        0.2f,  0.0f, -0.2f,  1.0f, 0.0f,
};

const GLfloat body_whole_vertices_[] = {
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

const GLuint body_indices_[] = {
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

SatelliteModel::SatelliteModel(GLFWwindow *window, std::string name)
{
    m_name = name;
    m_wings_elements_qty = sizeof(wings_indices_)/sizeof(GLint);
    m_body_elements_qty = sizeof(body_indices_)/sizeof(GLint);
}

SatelliteModel::~SatelliteModel()
{
    glDeleteVertexArrays(VAO_ARRAYS_QTY, &m_vao[0]);
    glDeleteBuffers(VAO_ARRAYS_QTY, &m_vbo[0]);
    glDeleteBuffers(VAO_ARRAYS_QTY, &m_ebo[0]);
}

void SatelliteModel::createModel(GLuint *vao, GLuint *vbo, GLuint *ebo)
{
    //Wings part buffer filling

    glGenVertexArrays(VAO_ARRAYS_QTY, &vao[0]);
    glGenBuffers(VAO_ARRAYS_QTY, &vbo[0]);
    glGenBuffers(VAO_ARRAYS_QTY, &ebo[0]);

    glBindVertexArray(vao[m_wings_array_index]);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[m_wings_array_index]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wings_vertices_), &wings_vertices_[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[m_wings_array_index]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wings_indices_), &wings_indices_[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Body part buffer filling
    glBindVertexArray(vao[m_body_array_index]);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[m_body_array_index]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(body_whole_vertices_), &body_whole_vertices_[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[m_body_array_index]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(body_indices_), &body_indices_[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

}

void SatelliteModel::renderWings(GLuint buf_index)
{


    glBindVertexArray(buf_index);
    glDrawElements(GL_TRIANGLES, sizeof(wings_indices_)/sizeof(GLint), GL_UNSIGNED_INT, 0);
}
void SatelliteModel::renderBody(GLuint buf_index)
{


    glBindVertexArray(buf_index);
    glDrawElements(GL_TRIANGLES, sizeof(body_indices_)/sizeof(GLint), GL_UNSIGNED_INT, 0);
}

glm::mat4 SatelliteModel::getModelMatrix()
{
    return m_model;
}

glm::mat4 SatelliteModel::rotateModel(float degrees, glm::vec3 axis_vec)
{
    m_model = glm::rotate(m_model, glm::radians(degrees), axis_vec);
    return m_model;
}

glm::mat4 SatelliteModel::translateModel(glm::vec3 translate_vec3)
{
    m_model = glm::translate(m_model,  translate_vec3);
    return m_model;
}

glm::mat4 SatelliteModel::scaleModel(glm::vec3 scale_vec3)
{
    m_model = glm::scale(m_model, scale_vec3);
    return m_model;
}