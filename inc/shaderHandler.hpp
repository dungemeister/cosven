#ifndef SHADER_HANDLER_HPP
#define SHADER_HANDLER_HPP

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <thread>



GLuint loadCubemap(std::vector<std::string> faces);
enum shaderType{
    vertex_shader = 0,
    fragment_shader,
};

enum compileErrorType{
    shaderCompileOk = 0,
    shaderCompileError,
    shaderLinkError,
};

class ShaderHandler
{
    

public:
    ShaderHandler(GLFWwindow *window, std::string shader_name);
    ~ShaderHandler();
    bool loadSource(std::string filepath, shaderType type);
    compileErrorType compileShader();
    bool getShaderCompileError(shaderType type);
    void useProgram();
    bool loadTexture(std::string texture_path, bool alpha_channel);
    void loadUniformInt(std::string uniform_name, GLint value);
    void loadUniformFloat(std::string uniform_name, GLfloat value);
    void loadUniformMatrix4fv(std::string uniform_name, const glm::mat4 &mat);
    void loadUniform3fv(std::string uniform_name, const glm::vec3 &vec);
    GLuint getProgramId() {return m_program_id;};
    GLuint getTextId(int text_index) {return m_text_id.at(text_index);};
    GLuint getTexCount() {return m_text_id.size();}

private:
    std::string m_shader_name;
    GLuint m_program_id;
    GLuint m_vertex_shader_id;
    GLuint m_fragment_shader_id;
    bool m_load_error = false;
    std::string m_error_msg;
    std::string m_vertex_shader_source;
    std::string m_fragment_shader_source;
    std::vector<GLuint> m_text_id;

};  

#endif //SHADER_HANDLER_HPP