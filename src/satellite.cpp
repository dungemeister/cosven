#include "satellite.hpp"


Satellite::Satellite(GLFWwindow *window, std::string name)
{
    
    m_name = name;
    m_window = window;
    m_program = new ShaderHandler(window, "program " + name);
    m_model = new SatelliteModel(window, name);
}

Satellite::~Satellite()
{

}

bool Satellite::loadShaderSource(std::string filepath, shaderType type)
{
    return m_program->loadSource(filepath, type);
}

bool Satellite::compileShader()
{
    return m_program->compileShader();
}

void Satellite::loadShaderTexture(std::string filepath, bool alpha_channel)
{
    m_program->loadTexture(filepath, alpha_channel);
    // m_program->loadUniformInt("u_texture", 0);
}

void Satellite::useShaderProgram()
{
    m_program->useProgram();
}

void Satellite::createModel()
{
    m_model->createModel(m_vao, m_vbo, m_ebo);
    m_program->loadUniformInt("u_texture", 0);
}

void Satellite::renderModel()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_program->getTextId(0));

    m_program->loadUniformMatrix4fv("model", m_model->getModelMatrix());
    if(m_fill == POLYGON_MODE_FILL)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if(m_fill == POLYGON_MODE_LINE)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if(m_fill == POLYGON_MODE_POINT)
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

    m_model->renderWings(m_vao[0]);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_program->getTextId(1));
    m_model->renderBody(m_vao[1]);

}
void Satellite::loadShaderUniformInt(std::string uniform_name, GLuint value)
{
    glUniform1i(glGetUniformLocation(m_program->getProgramId(), uniform_name.c_str()), value);
}

void Satellite::loadShaderUniformFloat(std::string uniform_name, GLfloat value)
{
    glUniform1f(glGetUniformLocation(m_program->getProgramId(), uniform_name.c_str()), value);

}

void Satellite::loadShaderProjectionMatrix(glm::mat4 projection)
{
    m_program->loadUniformMatrix4fv("projection", projection);
}

void Satellite::loadShaderModelMatrix(glm::mat4 model)
{
    m_program->loadUniformMatrix4fv("model", model);
}

void Satellite::loadShaderViewMatrix(glm::mat4 view)
{
    m_program->loadUniformMatrix4fv("view", view);
}

void Satellite::translateModel(glm::vec3 translate_vec)
{
    m_model->translateModel(translate_vec);
}

void Satellite::rotateModel(float degree, glm::vec3 axis_vec)
{
    m_model->rotateModel(degree, axis_vec);
}

void Satellite::scaleModel(glm::vec3 scale_vec)
{
    m_model->scaleModel(scale_vec);
}

void Satellite::setPolygonMode(bool fill_mode)
{
    if(fill_mode)
        m_fill = POLYGON_MODE_FILL;
    else
        m_fill = POLYGON_MODE_LINE;
}