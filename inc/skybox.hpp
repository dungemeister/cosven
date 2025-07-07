#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include "shaderHandler.hpp"

class Skybox{
public:
    Skybox(GLFWwindow *window, std::string name);
    ~Skybox();
    void createModel();
    void createCubemapModel();
    void render(glm::mat4 projection, glm::mat4 view);
    bool loadShaderSource(std::string filepath, shaderType type);
    bool compileShader();
    void useShaderProgram();
    void loadSkyboxCubemap(std::vector<std::string> surfaces);
    void loadUniformInt(std::string uniform_name, GLint value);
    void loadUniformMatrix4fv(std::string uniform_name, const glm::mat4 &mat);
private:
    GLuint m_vao[1] = {};
    GLuint m_vbo[1] = {};
    GLuint m_ebo[1] = {};
    GLuint m_text_id;
    ShaderHandler *m_program;
    std::vector<std::string> m_surfaces = {};
};

// Класс для skybox
class NewSkybox {
public:
    GLuint vao, vbo, ebo, cubemapTexture;
    std::vector<float> m_vertices;
    std::vector<unsigned int> m_indices;

    NewSkybox(const std::vector<std::string>& faces) {
        GenerateSkybox();
        cubemapTexture = LoadCubemap(faces);

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
        // CheckGLError("Skybox Setup");
    }

    void Render(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection) {
        glDepthMask(GL_FALSE); // Отключаем запись в буфер глубины
        glDepthFunc(GL_LEQUAL); // Skybox рендерится на максимальной глубине
        glUseProgram(shaderProgram);

        // Удаляем трансляцию из view-матрицы
        glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));
        glm::mat4 mvp = projection * viewNoTranslation;
        GLint mvpLoc = glGetUniformLocation(shaderProgram, "mvp");
        if (mvpLoc == -1) printf("Ошибка: униформа 'mvp' не найдена\n");
        else glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);

        GLint texLoc = glGetUniformLocation(shaderProgram, "skybox");
        if (texLoc == -1) printf("Ошибка: униформа 'skybox' не найдена\n");
        else glUniform1i(texLoc, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glDepthFunc(GL_LESS); // Возвращаем стандартный тест глубины
        glDepthMask(GL_TRUE); // Включаем запись в буфер глубины
        // CheckGLError("Skybox Render");
    }
    void GenerateSkybox(){
        m_vertices = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f
    };

    m_indices = {
        0, 1, 2, 2, 3, 0, // Передняя
        4, 5, 6, 6, 7, 4, // Задняя
        0, 4, 7, 7, 3, 0, // Верхняя
        1, 5, 6, 6, 2, 1, // Нижняя
        0, 1, 5, 5, 4, 0, // Левая
        3, 2, 6, 6, 7, 3  // Правая
    };
    }
    ~NewSkybox() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        glDeleteTextures(1, &cubemapTexture);
    }
private:
    GLuint LoadCubemap(const std::vector<std::string>& faces) {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, channels;
        for (unsigned int i = 0; i < faces.size(); i++) {
            unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
            if (data) {
                GLenum format = channels == 4 ? GL_RGBA : GL_RGB;
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                printf("Кубическая текстура загружена: %s (%dx%d, %d каналов)\n", faces[i].c_str(), width, height, channels);
                stbi_image_free(data);
            } else {
                printf("Ошибка загрузки кубической текстуры: %s\n", faces[i].c_str());
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        // CheckGLError("LoadCubemap");
        return textureID;
    }

};

#endif //SKYBOX_HPP