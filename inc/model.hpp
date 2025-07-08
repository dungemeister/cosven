#ifndef SATELLITE_MODEL_HPP
#define SATELLITE_MODEL_HPP

#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define VAO_ARRAYS_QTY (2)

enum ModelPolygonMode{
    POLYGON_MODE_FILL = 0,
    POLYGON_MODE_LINE,
    POLYGON_MODE_POINT,
};

// Класс для одной части спутника
class Model {
public:
    GLuint vao, vbo, ebo, instanceVBO, texture;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<glm::mat4> instanceTransforms; // Заменяем instancePositions на матрицу модели для каждого инстанса

    Model(const std::vector<float>& verts, const std::vector<unsigned int>& idx, const std::string& texturePath);

    void AddInstance(const glm::mat4& pos);
    void RemoveInstance();
    void RemoveAllInstances();
    void UpdateInstanceBuffer();
    void Render(GLuint shaderProgram, const glm::mat4& mvp, const glm::mat4& offsetModel);

    ~Model() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        glDeleteBuffers(1, &instanceVBO);
        glDeleteTextures(1, &texture);
    }
private:
    void CheckGLError(const char* operation);
    GLuint LoadTexture(const char* filepath);
};

#endif //SATELLITE_MODEL_HPP