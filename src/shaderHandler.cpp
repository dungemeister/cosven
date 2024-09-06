#include "shaderHandler.hpp"


#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

ShaderHandler::ShaderHandler(GLFWwindow *window, std::string shader_name)
{
    m_shader_name = shader_name;
}

ShaderHandler::~ShaderHandler()
{

}
bool ShaderHandler:: loadSource(std::string filepath, shaderType type)
{
    std::string content;

    std::ifstream fileStream(filepath, std::ios::in);
    std::string line = "";
    if (!fileStream.is_open())
    {
        std::cout << "Fail to load source file " << filepath << std::endl;
        return false;
    }
    while(!fileStream.eof())
    {
        getline(fileStream, line);
        content.append(line);
        content.append("\n");
    }
    fileStream.close();

    if(type == vertex_shader)
    {
        m_vertex_shader_source = content;
    }
    if(type == fragment_shader)
    {
        m_fragment_shader_source = content;
    }
    return true;
}

compileErrorType ShaderHandler::compileShader()
{
    const char * vshaderChars = m_vertex_shader_source.c_str();
    const char * fshaderChars = m_fragment_shader_source.c_str();

    m_vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    m_fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(m_vertex_shader_id, 1, &vshaderChars, NULL);
    glShaderSource(m_fragment_shader_id, 1, &fshaderChars, NULL);

    glCompileShader(m_vertex_shader_id);
    glCompileShader(m_fragment_shader_id);

    if(getShaderCompileError(vertex_shader))
    {
        return shaderCompileError;
    }
    if(getShaderCompileError(fragment_shader))
    {
        return shaderCompileError;
    }

    GLchar info[512];
    GLint link_status;
    m_program_id = glCreateProgram();
    glAttachShader(m_program_id, m_vertex_shader_id);
    glAttachShader(m_program_id, m_fragment_shader_id);
    glLinkProgram(m_program_id);

    glGetProgramiv(m_program_id, GL_LINK_STATUS, &link_status);
    if(!link_status)
    {
        glGetProgramInfoLog(m_program_id, 512, NULL, info);
        std::cout << m_shader_name << ": " << info << std::endl;
        return shaderLinkError;
    }
    return shaderCompileOk;
}

void ShaderHandler::useProgram()
{
    glUseProgram(m_program_id);
}

bool ShaderHandler::loadTexture(std::string texture_path, bool alpha_channel)
{
    int width, height, channels;
    GLuint text_id;
    glGenTextures(1, &text_id);
    m_text_id.push_back(text_id);
    glBindTexture(GL_TEXTURE_2D, text_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // stbi_set_flip_vertically_on_load(true);

    unsigned char *logo_texture_data = stbi_load(texture_path.c_str(), &width, &height, &channels, 0);
    if(logo_texture_data)
    {
        if(alpha_channel)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, logo_texture_data);

        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, logo_texture_data);
        glGenerateMipmap(GL_TEXTURE_2D);

    }
    else{
        std::cout << "fail to load texture" << std::endl;
        m_text_id.pop_back();
        return false;
    }

    stbi_image_free(logo_texture_data);
    return true;
}

bool ShaderHandler::getShaderCompileError(shaderType type)
{
    int len = 0;
    int chWritten = 0;
    char *log;
    GLuint shader_id;
    if(type == vertex_shader)
        shader_id = m_vertex_shader_id;
    if(type == fragment_shader)
        shader_id = m_fragment_shader_id;

    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &len);
    if(len > 0)
    {
        log = (char *)malloc(len);
        glGetShaderInfoLog(shader_id, len, &chWritten, log);
        std::cout << "Shader " << m_shader_name << (type == vertex_shader ? " vertex: ": " fragment: ") << log << std::endl;
        m_load_error = true;
        m_error_msg = std::string(log);
        free(log);
    }
    return m_load_error;
}

void ShaderHandler::loadUniformInt(std::string uniform_name, GLint value)
{
    glUniform1i(glGetUniformLocation(m_program_id, uniform_name.c_str()), value);
}

void ShaderHandler::loadUniformFloat(std::string uniform_name, GLfloat value)
{
    glUniform1f(glGetUniformLocation(m_program_id, uniform_name.c_str()), value);

}

void ShaderHandler::loadUniformMatrix4fv(std::string uniform_name, const glm::mat4 &mat)
{
    glUniformMatrix4fv(glGetUniformLocation(m_program_id, uniform_name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ShaderHandler::loadUniform3fv(std::string uniform_name, const glm::vec3 &vec)
{
    glUniform3fv(glGetUniformLocation(m_program_id, uniform_name.c_str()), 1, &vec[0]);
}

GLuint loadCubemap(std::vector<std::string> faces)
{
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

    int width, height, nrChannels;
    for(int i = 0; i< faces.size(); i++)
    {
        GLubyte *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if(data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
            0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else{
            std::cout << "surface load error" << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureId;
}