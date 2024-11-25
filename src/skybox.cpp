#include "skybox.hpp"

#include <stb_image.h>
const GLfloat skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
const GLfloat cubeTextureCoord[] = {
    //Front
    -1.0f,  0.0f, 1.0f,  0.25f, 0.33f, //0
    -1.0f,  1.0f, 1.0f,  0.25f, 0.66f, //1
    1.0f,  1.0f, 1.0f,  0.5f, 0.66f,  //2
    1.0f,  0.0f, 1.0f,  0.5f, 0.33f,  //3
    //Back
    -1.0f,  0.0f, -1.0f,  1.0f, 0.33f,//4
    -1.0f,  1.0f, -1.0f,  1.0f, 0.66f,//5
    1.0f,  1.0f, -1.0f,  0.75f, 0.66f, //6
    1.0f,  0.0f, -1.0f,  0.75f, 0.33f, //7
    //Left
    -1.0f,  0.0f, 1.0f,  0.25f, 0.33f, //8
    -1.0f,  1.0f, 1.0f,  0.25f, 0.66f, //9
    -1.0f,  1.0f, -1.0f,  0.0f, 0.66f,//10
    -1.0f,  0.0f, -1.0f,  0.0f, 0.33f,//11
    //Right
    1.0f,  0.0f, 1.0f,  0.5f, 0.33f,  //12
    1.0f,  1.0f, 1.0f,  0.5f, 0.66f,  //13
    1.0f,  1.0f, -1.0f,  0.75f, 0.66f, //14
    1.0f,  0.0f, -1.0f,  0.75f, 0.33f, //15
    //Bottom
    -1.0f,  0.0f, 1.0f,  0.25f, 0.33f, //16
    1.0f,  0.0f, 1.0f,  0.5f, 0.33f,  //17
    1.0f,  0.0f, -1.0f,  0.5f, 0.0f, //18
    -1.0f,  0.0f, -1.0f,  0.25f, 0.0f,//19
    //Top
    -1.0f,  1.0f, 1.0f,  0.25f, 0.66f, //20
    1.0f,  1.0f, 1.0f,  0.5f, 0.66f,  //21
    1.0f,  1.0f, -1.0f,  0.5f, 1.0f, //22
    -1.0f,  1.0f, -1.0f,  0.25f, 1.0f,//23
}; 

const GLuint skybox_indices[] = {
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

Skybox::Skybox(GLFWwindow *window, std::string name)
{
    m_program = new ShaderHandler(window, "program " + name);
    createCubemapModel();

}

bool Skybox::loadShaderSource(std::string filepath, shaderType type)
{
    return m_program->loadSource(filepath, type);
}

bool Skybox::compileShader()
{
    return m_program->compileShader();
}

void Skybox::useShaderProgram()
{
    m_program->useProgram();
}

void Skybox::createModel()
{
    glGenVertexArrays(1, &m_vao[0]);
    glGenBuffers(1, &m_vbo[0]);
    glGenBuffers(1, &m_ebo[0]);

    glBindVertexArray(m_vao[0]);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTextureCoord), &cubeTextureCoord[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skybox_indices), &skybox_indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    int width, height, channels;
    glGenTextures(1, &m_text_id);
    glBindTexture(GL_TEXTURE_2D, m_text_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // stbi_set_flip_vertically_on_load(true);

    unsigned char *logo_texture_data = stbi_load("/home/yura/opengl/skybox/skybox.png", &width, &height, &channels, 0);
    if(logo_texture_data)
    {

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, logo_texture_data);
        glGenerateMipmap(GL_TEXTURE_2D);

    }
    else{
        std::cout << "fail to load texture" << std::endl;
    }

    stbi_image_free(logo_texture_data);
}

void Skybox::createCubemapModel()
{
    glGenVertexArrays(1, &m_vao[0]);
    glGenBuffers(1, &m_vbo[0]);

    glBindVertexArray(m_vao[0]);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);
}

Skybox::~Skybox()
{
    glDeleteBuffers(1, m_vao);
    glDeleteBuffers(1, m_vbo);
    glDeleteBuffers(1, m_ebo);
}

void Skybox::render(glm::mat4 projection, glm::mat4 view)
{
    loadUniformMatrix4fv("view", view);
    loadUniformMatrix4fv("projection", projection);
    useShaderProgram();
    
    glDepthFunc(GL_LEQUAL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(m_vao[0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_text_id);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(skyboxVertices)/(sizeof(GLfloat) * 3));
    
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);

}
void Skybox::loadSkyboxCubemap(std::vector<std::string> surfaces)
{
    m_surfaces = surfaces;
    m_text_id = loadCubemap(m_surfaces);
    useShaderProgram();
    loadUniformInt("skybox", 0);
}

void Skybox::loadUniformInt(std::string uniform_name, GLint value)
{
    m_program->loadUniformInt(uniform_name, value);
}

void Skybox::loadUniformMatrix4fv(std::string uniform_name, const glm::mat4 &mat)
{
    m_program->loadUniformMatrix4fv(uniform_name, mat);
}