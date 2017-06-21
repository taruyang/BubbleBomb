#include "camera.h"
#include <stdio.h>
#include <glm/gtc/quaternion.hpp>

namespace gl
{

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp)
{
    /** Default projection mode and matrix */
    _updateProj = false;
    _projMode   = PROJ_NONE;
    _projMat    = glm::mat4(1);

    /** Default view mode and matrix  */
    _updateView = false;
    _viewMat = glm::mat4(1);

    _movementSpeed = SPEED;
    _mouseSensitivity = SENSITIVTY;
    _yaw = YAW;
    _pitch = PITCH;
}

void Camera::LookAt(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp)
{
    _position = position;
    _target = target;
    _worldUp = worldUp;

    _front = glm::normalize(target - position);

    updateCoordinate();
}

void Camera::Shift(glm::vec3 movement)
{
    _position += movement;
    _target += movement;

    _front = glm::normalize(_target - _position);

    updateCoordinate();

}

void Camera::ProcessKeyboard(CameraMovement direction, GLfloat timeDiff)
{
    GLfloat velocity = _movementSpeed * timeDiff;
    if (direction == MOVE_FORWARD)
        _position += _front * velocity;
    if (direction == MOVE_BACKWARD)
        _position -= _front * velocity;

    /// Update Front, Right and Up Vectors using the updated Eular angles
    updateCoordinate();
}

void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)
{
    if(xoffset == 0.f && yoffset == 0.f)
        return;

    xoffset *= _mouseSensitivity;
    yoffset *= _mouseSensitivity;

    _yaw   += xoffset;
    _pitch += yoffset;

    /// printf("[mouse] xoff(%f), yoff(%f), yaw(%f), pitch(%f) \n", xoffset, yoffset, _yaw, _pitch);
    /// Make sure that when pitch is out of bounds, screen does not get flipped
    if (constrainPitch)
    {
        if (_pitch > 89.0f)
            _pitch = 89.0f;
        if (_pitch < -89.0f)
            _pitch = -89.0f;
    }

    /// Update Front, Right and Up Vectors using the updated Eular angles
    updateCoordinate();
}

/**
{
    glm::quat q = glm::angleAxis(5.0f, _up);
    q = glm::normalize(q);

    _front = glm::vec3(2.f * q.x * q.z - 2.f * q.w * q.y,
                       2.f * q.y * q.z + 2.f * q.w * q.x,
                       1.f - 2.f * q.x * q.x - 2.f * q.y * q.y);

     updateCoordinate();
}
*/

void Camera::ProcessMouseScroll(GLfloat yoffset)
{
    if(_vol.fov >= 1.0f && _vol.fov <= 45.0f)
        _vol.fov -= yoffset;
    if(_vol.fov <= 1.0f)
        _vol.fov = 1.0f;
    if(_vol.fov >= 45.0f)
        _vol.fov = 45.0f;

}

const glm::mat4 Camera::GetProjMatrix()
{
    if(!_updateProj)
        return _projMat;

    if(_projMode == PROJ_PERSPECTIVE)
    {
        float fov = glm::radians(_vol.fov);
        float f = 1.0f/tan(fov / 2.0f);
        float Sz = (_vol.far + _vol.near) / (_vol.near - _vol.far);
        float Pz = (2.0f * _vol.far * _vol.near) / (_vol.near - _vol.far);

        _projMat =  glm::mat4(f / _vol.aspect, 0, 0, 0,
                                    0, f, 0, 0,
                                    0, 0, Sz, -1.0f,
                                    0, 0, Pz, 0);

    }
    else
        _projMat = glm::mat4(1);

    _updateProj = false;

    return _projMat;
}

const glm::mat4 Camera::GetViewMatrix()
{
    if(!_updateView)
        return _viewMat;

    /// View matrix
    glm::mat4 viewRotation(
        _right.x, _up.x, -_front.x, 0.f,
        _right.y, _up.y, -_front.y, 0.f,
        _right.z, _up.z, -_front.z, 0.f,
        0.f,        0.f,        0.f,        1.f);

    glm::mat4 viewTranslation(
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        -_position.x, -_position.y, -_position.z, 1.f);

    _viewMat = viewRotation * viewTranslation;

    _updateView = false;

    return _viewMat;
}

void Camera::SetViewingFrustrum(GLfloat near, GLfloat far, GLfloat fov, GLfloat aspect)
{
    _updateProj = true;
    _vol.near = near;
    _vol.far = far;
    _vol.fov = fov;
    _vol.aspect = aspect;

    _projMode = PROJ_PERSPECTIVE;
}

void Camera::updateCoordinate()
{
    _updateView = true;

    /// Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    front.y = sin(glm::radians(_pitch));
    front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    _front = glm::normalize(front);

    _right = glm::normalize(glm::cross(_front, _worldUp));
    _up    = glm::normalize(glm::cross(_right, _front));
}
}
