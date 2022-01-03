#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.05f;
const float ZOOM = 70.0f;

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 TargetPos;
    float CamTarDist;

    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 target = glm::vec3(0.0f, 0.0f, -1.0f), float ctdist = 1.0f, 
        glm::vec3 worldup = glm::vec3(0.0f, 1.0f, 0.0f))
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = worldup;
        TargetPos = target;
        CamTarDist = ctdist;
        updateCameraVectors();
    }

    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float tarX, float tarY, float tarZ, float ctdist, 
        float worldupX, float worldupY, float worldupZ)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(worldupX, worldupY, worldupZ);
        TargetPos = glm::vec3(tarX, tarY, tarZ);
        CamTarDist = ctdist;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, TargetPos, Up);
    }

    void ProcessKeyboard(glm::vec3 target)
    {
        TargetPos = target;
        updateCameraVectors();
        Position = TargetPos - CamTarDist * Front;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean allowPitch = false, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        glm::vec4 TarToCamVec = glm::vec4(Position - TargetPos, 0.0f);
        glm::mat4 rotateMat = glm::mat4(1.0f);
        //rotateMat = glm::rotate(rotateMat, glm::radians(yoffset), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateMat = glm::rotate(rotateMat, glm::radians(-xoffset), glm::vec3(0.0f, 1.0f, 0.0f));
        if (allowPitch) {
            if (std::abs(xoffset) < std::abs(yoffset)) {
                rotateMat = glm::rotate(rotateMat, glm::radians(yoffset), Right);
            }
        }
        TarToCamVec = rotateMat * TarToCamVec;
        Position = TargetPos + glm::vec3(TarToCamVec.x, TarToCamVec.y, TarToCamVec.z);
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 90.0f)
            Zoom = 90.0f;
    }

private:

    void updateCameraVectors()
    {
        Front = glm::normalize(TargetPos - Position);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }

};

#endif