#ifndef ORBIT_CAMERA_HPP
#define ORBIT_CAMERA_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera{
public:
    virtual ~Camera() = default;
    virtual void Update(float deltatime) = 0;
    
    glm::mat4 GetViewMatrix(){
        return glm::lookAt(position, target, up);
    }
    
    glm::mat4 GetProjectionMatrix() {
        return glm::perspective(fov, aspect, nearClip, farClip);
    }
    glm::vec3 GetPosition() { return position; }
    float GetFov() { return fov; }
    float GetAspect() { return aspect; }

protected:
    glm::vec3 position; // Позиция камеры
    glm::vec3 target;   // Точка, на которую смотрит камера
    glm::vec3 up;       // Вектор "вверх" для ориентации

    float fov;          // Угол обзора (Field of View)
    float aspect;       // Соотношение сторон
    float nearClip;     // Ближняя плоскость отсечения
    float farClip;      // Дальняя плоскость отсечения
};

class OrbitCamera: public Camera{
public:    
    OrbitCamera()
    {
        distance = 20.0f;
        angle = 0.0f;

        position = glm::vec3(distance*cos(angle), 0.0f, distance*sin(angle));
        target = glm::vec3(0.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        fov = glm::radians(45.0f);
        aspect = 1920.0f / 1080.0f;
        nearClip = 0.1f;
        farClip = 100.0f;
    }
    ~OrbitCamera() = default;
    
    void Update(float coef) override{
        angle += coef;
        position.x = target.x + distance * cos(angle);
        position.z = target.z + distance * sin(angle);
    }
    float GetDistance() { return distance; }
    void SetDistance(float dist) {
        distance = dist;
        position = glm::vec3(distance*cos(angle), 0.0f, distance*sin(angle));
    }
    void IncreaseDistance(float dist){
        distance -= dist;
        position = glm::vec3(distance*cos(angle), 0.0f, distance*sin(angle));
    }
    void UpdateAngle(float delta_angle){
        angle -= delta_angle * MOUSE_SENSITIVITY;
        position = glm::vec3(distance*cos(angle), 0.0f, distance*sin(angle));
    }

private:
    float distance;
    float angle;

    float MOUSE_SENSITIVITY =  0.005f;
};

#endif //ORBIT_CAMERA_HPP