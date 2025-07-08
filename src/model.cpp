#include "model.hpp"

#include <stb_image.h>

Model::Model(const std::vector<float>& verts, const std::vector<unsigned int>& idx, const std::string& texturePath) {
        vertices = verts;
        indices = idx;
        texture = LoadTexture(texturePath.c_str());
        if (texture == 0) {
            printf("Не удалось загрузить текстуру для модели\n");
        }

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glGenBuffers(1, &instanceVBO);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Настройка instanceVBO для матриц
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glBindVertexArray(0);
        CheckGLError("Model Setup");
    }

// Проверка ошибок OpenGL
void Model::CheckGLError(const char* operation) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) printf("OpenGL Error after %s: %d\n", operation, err);
}

// Загрузка текстуры
GLuint Model::LoadTexture(const char* filepath) {
    int width, height, channels;
    unsigned char* data = stbi_load(filepath, &width, &height, &channels, 0);
    if (!data) {
        printf("Ошибка загрузки текстуры: %s\n", filepath);
        return 0;
    }
    GLenum format = channels == 4 ? GL_RGBA : GL_RGB;
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    CheckGLError("glTexImage2D");
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    printf("Текстура загружена: %s (%dx%d, %d каналов)\n", filepath, width, height, channels);
    return texture;
}

void Model::AddInstance(const glm::mat4& transform) {
    instanceTransforms.push_back(transform);
    UpdateInstanceBuffer();
}
void Model::RemoveInstance(){
    instanceTransforms.pop_back();
    UpdateInstanceBuffer();
}
void Model::RemoveAllInstances(){
    instanceTransforms.clear();
    UpdateInstanceBuffer();
}
void Model::UpdateInstanceBuffer() {
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, instanceTransforms.size() * sizeof(glm::mat4), instanceTransforms.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CheckGLError("UpdateInstanceBuffer");
}

void Model::Render(GLuint shaderProgram, const glm::mat4& mvp, const glm::mat4& offsetModel) {
    glUseProgram(shaderProgram);
    GLint mvpLoc = glGetUniformLocation(shaderProgram, "mvp");
    if (mvpLoc == -1) {
        printf("Ошибка: униформа 'mvp' не найдена\n");
    } else {
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);
    }
    
    GLint offsetLoc = glGetUniformLocation(shaderProgram, "offsetModel");
    if (offsetLoc == -1) {
        printf("Ошибка: униформа 'mvp' не найдена\n");
    } else {
        glUniformMatrix4fv(offsetLoc, 1, GL_FALSE, &offsetModel[0][0]);
    }

    GLint texLoc = glGetUniformLocation(shaderProgram, "texture1");
    if (texLoc == -1) {
        printf("Ошибка: униформа 'texture1' не найдена\n");
    } else {
        glUniform1i(texLoc, 0);
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(vao);
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instanceTransforms.size());
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    CheckGLError("Model Render");
}