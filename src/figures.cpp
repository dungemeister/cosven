#include "figures.hpp"

Torus::Torus(GLFWwindow *window, float ext_radius, float internal_radius, int ext_step, int int_step,
bool texture)
{
    m_window = window;
    m_program = new ShaderHandler(window, "program torus");
    m_texture = texture;

    m_ext_step = ext_step;
    m_int_step = int_step;
    m_ext_radius = ext_radius;
    m_int_radius = internal_radius;

    m_external_qty = 360/m_ext_step + 1;
    m_internal_qty = 360/m_int_step + 1;
    m_vertices_qty = m_external_qty * m_internal_qty;
    m_indices_qty = 2 * (m_vertices_qty - m_internal_qty);

    m_indices = (GLuint*)malloc(m_indices_qty * sizeof(GLuint));
    m_vertices = (vertice*)malloc(m_vertices_qty * sizeof(vertice));

    std::cout << "torus calcs" << std::endl;

    for(int tetha = 0, index = 0; tetha <= 360; tetha += m_ext_step)
    {
        float ext_angle = tetha % 360;
        for(int phi = 0; phi <= 360; phi += m_int_step, index++)
        {
            float int_angle = phi % 360;

            m_vertices[index].x = glm::cos(glm::radians(ext_angle)) * (m_ext_radius + 
            m_int_radius * glm::cos(glm::radians(int_angle)));

            m_vertices[index].y = glm::sin(glm::radians(ext_angle)) * (m_ext_radius + 
            m_int_radius * glm::cos(glm::radians(int_angle)));

            m_vertices[index].z = glm::sin(glm::radians(int_angle)) * m_int_radius;

        }
    }

    for(int index = 0; index < m_indices_qty; index += 2)
    {
        m_indices[index] = index >> 1;
        m_indices[index + 1] = m_indices[index] + (360/m_int_step) + 1;

    }

    if(m_texture)
        createModelWithTexture();
    else
        createModel();
}

void Torus::createModel()
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices_qty * sizeof(vertice), &m_vertices[0], GL_STATIC_DRAW);
    std::cout << sizeof(m_vertices) << std::endl;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices_qty * sizeof(GLuint), &m_indices[0], GL_STATIC_DRAW);
    std::cout << sizeof(m_indices) << std::endl;

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);

    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void Torus::createModelWithTexture()
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), &m_vertices[0], GL_STATIC_DRAW);
    std::cout << sizeof(m_vertices) << std::endl;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices), &m_indices[0], GL_STATIC_DRAW);
    std::cout << sizeof(m_indices) << std::endl;

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}
bool Torus::loadShaderSource(std::string filepath, shaderType type)
{
    return m_program->loadSource(filepath, type);
}

bool Torus::compileShader()
{
    return m_program->compileShader();
}

void Torus::loadShaderTexture(std::string filepath, bool alpha_channel)
{
    m_program->loadTexture(filepath, alpha_channel);
    // m_program->loadUniformInt("u_texture", 0);
}

void Torus::useShaderProgram()
{
    m_program->useProgram();
}

void Torus::render(glm::mat4 projection, glm::mat4 view)
{
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, m_program->getTextId(0));
    useShaderProgram();
    glDepthFunc(GL_LESS);
    
    if(m_polygon_mode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    loadShaderProjectionMatrix(projection);
    loadShaderViewMatrix(view);
    m_program->loadUniformMatrix4fv("model", m_model);
    glBindVertexArray(m_vao);

    glDrawElements(GL_TRIANGLE_STRIP, m_indices_qty, GL_UNSIGNED_INT, 0);
    // glDrawArrays(GL_TRIANGLES, 0, m_vertices_qty);
}

void Torus::loadShaderUniformInt(std::string uniform_name, GLint value)
{
    glUniform1i(glGetUniformLocation(m_program->getProgramId(), uniform_name.c_str()), value);
}

void Torus::loadShaderUniformFloat(std::string uniform_name, GLfloat value)
{
    glUniform1f(glGetUniformLocation(m_program->getProgramId(), uniform_name.c_str()), value);

}

void Torus::loadShaderProjectionMatrix(glm::mat4 projection)
{
    m_program->loadUniformMatrix4fv("projection", projection);
}

void Torus::loadShaderModelMatrix(glm::mat4 model)
{
    m_program->loadUniformMatrix4fv("model", model);
}

void Torus::loadShaderViewMatrix(glm::mat4 view)
{
    m_program->loadUniformMatrix4fv("view", view);
}

void Torus::loadShaderUniformVec3(std::string uniform_name, glm::vec3 vec)
{
    m_program->loadUniform3fv(uniform_name, vec);
}

void Torus::rotate(float degree, glm::vec3 axis_vec)
{
    m_model = glm::rotate(m_model, glm::radians(degree), axis_vec);
}