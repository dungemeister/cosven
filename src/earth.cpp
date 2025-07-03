#include "earth.hpp"

Earth::Earth(GLFWwindow *window, GLfloat radius)
{
    m_radius = radius;
    m_window = window;
    m_program = new ShaderHandler(window, "program earth");

    for(int alpha = 90, index = 0; alpha <= 270; alpha += VER_STEP_ANGLE)
    {
        float verAngle = alpha % 360;
        
        // std::cout << "ring " << std::endl;
        for(int phi = 0; phi <= 360; phi += HOR_STEP_ANGLE, ++index)
        {
            float horAngle = phi % 360;
            m_vertices[index].y = m_radius * glm::sin(glm::radians(verAngle));
            m_vertices[index].x = m_radius * glm::cos(glm::radians(verAngle)) * glm::sin(glm::radians(horAngle));
            m_vertices[index].z = m_radius * glm::cos(glm::radians(verAngle)) * glm::cos(glm::radians(horAngle));

            // std::cout << "index " << index << " x " << m_vertices[index].x << " y " << m_vertices[index].y << " z " << m_vertices[index].z << std::endl;

            m_vertices[index].u = (360 - phi ) / 360.0f;
            m_vertices[index].v = (alpha - 90) / 180.0f;

            // if(alpha <= 90)
            //     m_vertices[index].v = 0.5f + alpha/180.0f;
            // else
            //     m_vertices[index].v = 0.5f - alpha/180.0f;

            // std::cout << " u " << m_vertices[index].u << " v " << m_vertices[index].v << std::endl;
        }
    }

    for(int index = 0; index < m_indices_qty; index += 2)
    {
        m_indices[index] = index >> 1;
        m_indices[index + 1] = m_indices[index] + (360/HOR_STEP_ANGLE) + 1;

    }

    std::cout << "";
}

Earth::~Earth()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
}

void Earth::createModel()
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), &m_vertices[0], GL_STATIC_DRAW);
    // std::cout << sizeof(m_vertices) << std::endl;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices), &m_indices[0], GL_STATIC_DRAW);
    // std::cout << sizeof(m_indices) << std::endl;

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);

}

bool Earth::loadShaderSource(std::string filepath, shaderType type)
{
    return m_program->loadSource(filepath, type);
}

bool Earth::compileShader()
{
    return m_program->compileShader();
}

void Earth::loadShaderTexture(std::string filepath, bool alpha_channel)
{
    m_program->loadTexture(filepath, alpha_channel);
    // m_program->loadUniformInt("u_texture", 0);
}
void Earth::deleteShaderTexture()
{
    m_program->deleteTexture();
}
void Earth::useShaderProgram()
{
    m_program->useProgram();
}

void Earth::render()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_program->getTextId(0));
    
    m_program->loadUniformMatrix4fv("model", m_model);
    glBindVertexArray(m_vao);

    glDrawElements(GL_TRIANGLE_STRIP, m_indices_qty, GL_UNSIGNED_INT, 0);
    // glDrawArrays(GL_TRIANGLES, 0, m_vertices_qty);
}

void Earth::loadShaderUniformInt(std::string uniform_name, GLint value)
{
    glUniform1i(glGetUniformLocation(m_program->getProgramId(), uniform_name.c_str()), value);
}

void Earth::loadShaderUniformFloat(std::string uniform_name, GLfloat value)
{
    glUniform1f(glGetUniformLocation(m_program->getProgramId(), uniform_name.c_str()), value);

}

void Earth::loadShaderProjectionMatrix(glm::mat4 projection)
{
    m_program->loadUniformMatrix4fv("projection", projection);
}

void Earth::loadShaderModelMatrix(glm::mat4 model)
{
    m_program->loadUniformMatrix4fv("model", model);
}

void Earth::loadShaderViewMatrix(glm::mat4 view)
{
    m_program->loadUniformMatrix4fv("view", view);
}

void Earth::loadShaderUniformVec3(std::string uniform_name, glm::vec3 vec)
{
    m_program->loadUniform3fv(uniform_name, vec);
}

void Earth::rotate(float degree, glm::vec3 axis_vec)
{
    m_model = glm::rotate(m_model, glm::radians(degree), axis_vec);
}