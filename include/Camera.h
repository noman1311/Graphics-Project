#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;

    // Default constructor: eye at (0,5,15), looking at origin.
    Camera(glm::vec3 position = glm::vec3(0.0f, 5.0f, 15.0f),
           glm::vec3 target   = glm::vec3(0.0f, 0.0f, 0.0f))
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
          WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
          Yaw(-90.0f), Pitch(0.0f)
    {
        Position = position;
        // Compute Yaw/Pitch from the target so the look direction is honored.
        glm::vec3 dir = glm::normalize(target - position);
        Yaw   = glm::degrees(atan2f(dir.z, dir.x));
        Pitch = glm::degrees(asinf(dir.y));
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};

#endif
