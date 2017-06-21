#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#define GLEW_NO_GLU
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace gl
{

/**
 * @brief   Structure to describe a viewing frustum in the perspective projection.
 */
struct ViewVol
{
    GLfloat near;
    GLfloat far;
    GLfloat fov;
    GLfloat aspect;
};

/**
 * @brief   Projection mode. Currently, the orthographic projection is not supported.
 */
enum ProjectMode {
    PROJ_NONE,
    PROJ_PERSPECTIVE
};

enum CameraMovement {
    MOVE_FORWARD,
    MOVE_BACKWARD,
    MOVE_LEFT,
    MOVE_RIGHT
};

/// Default camera values
const GLfloat YAW        = 90.0f;
const GLfloat PITCH      =  0.0f;
const GLfloat SPEED      =  10.0f;
const GLfloat SENSITIVTY =  0.10f;

/**
 * @brief   Class to manage and control a virtual camera the world space.
 *          After setting camera properties and a viewing frustum, users can retrieve
 *          related view and projection matrices.
 *          If the methods for the matrices are called without setting,
 *          those return 4x4 identity matrix.
 *          This class would be extended to support movements like work around and
 *          looking around, such as pitch and yaw.
 */
class Camera
{
    /** Camera vector Attributes */
    glm::vec3 _position;
    glm::vec3 _target;
    glm::vec3 _front;
    glm::vec3 _up;
    glm::vec3 _right;
    glm::vec3 _worldUp;

    /** camera options */
    GLfloat     _movementSpeed;
    GLfloat     _mouseSensitivity;

    /// Eular Angles
    GLfloat     _yaw;
    GLfloat     _pitch;

    /** View mode related information */
    bool        _updateView;
    glm::mat4   _viewMat;

    /** Projection mode */
    bool            _updateProj;
    ProjectMode     _projMode;
    glm::mat4       _projMat;
    ViewVol         _vol;

public:
    /**
     * @brief   Constructor
     *          All values can be changed by the LookAt method after.
     *
     * @param position  camera position in world coordinates.
     * @param target    The position of the target this camera object is looking at.
     * @param worldUp   Up vector in world space.
     */
    Camera(glm::vec3 position = glm::vec3(0.0f), glm::vec3 target = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 worldUp = glm::vec3(0.0f, 0.1f, 0.0f));

    /**
     * @brief   Setting camera vectors
     *
     * @param position  camera position in world coordinates.
     * @param target    The position of the target this camera object is looking at.
     * @param worldUp   Up vector in world space.
     */
    void LookAt(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp = glm::vec3(0.0f, 0.1f, 0.0f));

    /**
     * @brief  Shift camera
     *         This changes the camera position and target position together
     *
     * @param movement  the movement of this camera
     */
    void Shift(glm::vec3 movement);

    /**
     * @brief   Processes input received from keyboard.
     *          Accepts input parameter in the form of defined ENUM for camera movement
     *
     * @param direction  Direction of movement
     * @param timeDiff   Time difference between previous key press and current key press
     */
    void ProcessKeyboard(CameraMovement direction, GLfloat timeDiff);

    /**
     * @brief   Processes input received from a mouse input system.
     *          Expects the offset value in both the x and y direction.
     *
     */
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true);

    /**
     * @brief   Processes input received from a mouse scroll-wheel event.
     *          Only requires input on the vertical wheel-axis
     *
     */
    void ProcessMouseScroll(GLfloat yoffset);

    /**
     * @brief   Get a projection matrix based on current camera setting.
     * @return  projection matrix
     */
    const glm::mat4 GetProjMatrix();

    /**
     * @brief   Get a view matrix based on current camera setting.
     * @return  view matrix
     */
    const glm::mat4 GetViewMatrix();

    /**
     * @brief   Setting a viewing frustum
     *
     * @param near      The near plane position in the viewing frustum.
     * @param far       The far plane position in the viewing frustum.
     * @param fov       The field of view value in the viewing frustum.
     * @param aspect    The aspect ratio of the view port.
     */
    void SetViewingFrustrum(GLfloat near, GLfloat far, GLfloat fov, GLfloat aspect);

    /**
     * @brief   Setting aspect Ratio
     *
     * @param aspect    The aspect ratio of the view port.
     */
    void SetAspectRatio(GLfloat aspect){
        _updateProj = true; _vol.aspect = aspect; };

    /**
     * @brief   Get current camera position
     *
     * @return  camera position vector
     */
    glm::vec3 GetPosition() { return _position; };

    /**
     * @brief   Get current target position
     *
     * @return  target position vector
     */
    glm::vec3 GetTarget() { return _target; };

    glm::vec3 GetDirection() { return _front;};

private:
    void updateCoordinate();
};

}
#endif /// CAMERA_H_INCLUDED
