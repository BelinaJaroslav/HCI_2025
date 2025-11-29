#pragma once

#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

class Camera
{
public:

    // Camera Attributes
    glm::vec3 position{};
    glm::vec3 front{};
    glm::vec3 right{}; 
    glm::vec3 up{}; // camera local UP vector

    GLfloat yaw = -90.0f;
    GLfloat pitch = 0.0f;
    GLfloat roll = 0.0f;
    
    // Camera options
    GLfloat movement_speed = 5.0f;
    GLfloat mouse_sensitivity = 0.25f;
    
    Camera()
        : position(0.0f, 0.0f, 3.0f),
        front(0.0f, 0.0f, -1.0f),
        up(0.0f, 1.0f, 0.0f)
    {
        update_camera_vectors();
    }
    Camera(glm::vec3 position)
        : position(position),
        front(0.0f, 0.0f, -1.0f),
        up(0.0f, 1.0f, 0.0f)
    {
        update_camera_vectors();
    }


    glm::mat4 get_view_matrix()
    {
        return glm::lookAt(this->position, this->position + this->front, this->up);
    }


    glm::vec3 process_input(GLFWwindow* window, GLfloat delta_time, bool is_camera_freeform)
    {
        glm::vec3 direction = glm::vec3(0.0f);
        GLfloat _movement_speed = movement_speed;

        if (is_camera_freeform) {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                direction += front;

            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                direction -= front;

            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                direction -= right;

            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                direction += right;

            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
                direction += up;

            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
                direction -= up;

            _movement_speed *= 2.0f;
        }
        else {
            glm::vec3 horizont_front(front.x, 0, front.z);
            glm::vec3 horizont_right(right.x, 0, right.z);
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                direction += horizont_front;
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                direction += -horizont_front;
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                direction += -horizont_right;
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                direction += horizont_right;
            }
        }

        if (glm::length(direction) == 0.0f)
            return glm::vec3(0.0f);

        return glm::normalize(direction) * _movement_speed * delta_time;
    }


    void process_mouse_movement(GLfloat xoffset, GLfloat yoffset, GLboolean constraintPitch = GL_TRUE)
    {
        xoffset *= this->mouse_sensitivity;
        yoffset *= this->mouse_sensitivity;

        this->yaw   += xoffset;
        this->pitch += yoffset;

        if (constraintPitch)
        {
            if (this->pitch > 89.0f)
                this->pitch = 89.0f;
            if (this->pitch < -89.0f)
                this->pitch = -89.0f;
        }

        this->update_camera_vectors();
    }


private:
    void update_camera_vectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
        front.y = sin(glm::radians(this->pitch));
        front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

        this->front = glm::normalize(front);
        this->right = glm::normalize(glm::cross(this->front, glm::vec3(0.0f, 1.0f, 0.0f)));
        this->up = glm::normalize(glm::cross(this->right, this->front));
    }
};
