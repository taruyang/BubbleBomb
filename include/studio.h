#ifndef STUDIO_H_INCLUDED
#define STUDIO_H_INCLUDED

#include <math.h>
#include <vector>
#include <glm/glm.hpp>
#define GLEW_NO_GLU
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "IGraphicObject.h"
#include "windowManager.h"
#include "shader.h"
#include "textRenderer.h"
#include "gameControl.h"
#include "studioEnv.h"

namespace gl
{
using namespace std;

/**
 * @brief   Struct to contain commands from a director ( Keyboard or mouse input )
 */
typedef struct command
{
    bool allowToRender;

    command()
    {
        allowToRender = true;
    };

} Command;


#define SHADER_NUM  2

/**
 * @brief   Class to manage all graphics objects and to show output onto the requested window.
 *
 *          This is a kind of virtual studio to shoot of graphic object's behavior.
 *          All actions of every objects are defined in the "Ready" method.
 *          If specific camera setting in the studio is required, please let you use "CameraSetting" method
 *          before calling "Shoot" method.
 */

class Studio
{
    /// Window
    GLFWwindow* _window;
    uint32_t    _w, _h;

    /// camera
    glm::mat4   _projMat;
    Camera      _camera;

    /// For camera control
    bool        _firstMouse = true;
    GLfloat     _prevX, _prevY;
    GLfloat     _deltaTime = 0.0f;
    GLfloat     _lastRenderTime = 0.0f;

    /// Studio environment
    StudioEnv   _studioEnv;

    /// Players
    vector<IGraphicObject*> _objs;
    IGraphicObject*         _pIndicator;
    TextRenderer*           _pTextRenderer;

    void OnStage();

    /// Shader
    vector<Shader*> _shaders;

    /// rendering
    void renderNextFrame(const double time, glm::mat4 matrix, StudioEnv& studioEnv);

    /// Objects rearrangement based on current status
    void checkObjectsOnStage(StudioEnv& studioEnv);
    /// Command
    Command     _command;

    /// For game
    GameControl             _gameControl;
    void displayGameStatus(const double time, glm::mat4 matrix, StudioEnv& studioEnv);
    void ProcessKeyCommand();
    void ProcessMouseCommand();
    void ProcessFrameChangeCommand();
    void Casting();

public:
    /**
     * @brief   Constructor of Studio object
     */
    Studio();

    /**
     * @brief   Destructor of Studio object
     */
    ~Studio();

    /**
     * @brief   Preparation for all graphics object to be displayed on the window.
     * @return  result of method
     */
    bool Ready();

    /**
     * @brief   Screen setting
     * @param w     The width of requested display window.
     * @param h     The height of requested display window.
     * @return  result of method
     */
    bool ScreenSetting(uint32_t w, uint32_t h);

    /**
     * @brief   Camera setting in this studio object
     *
     * @param position  camera position in world coordinates.
     * @param lookAt    The position of the target this camera object is looking at.
     * @param near      The near plane position in the viewing frustum.
     * @param far       The far plane position in the viewing frustum.
     * @param fov       The field of view value in the viewing frustum.
     * @return  result of method
     */
    bool CameraSetting(glm::vec3 camPos, glm::vec3 lookAt, float near = 0.1f, float far = 200.0f, float fov = 67.0f);

    /**
     * @brief   Lighting setting in this studio object
     *
     * @param lightPos      lighting position in world coordinates.
     */
    void LightingSetting(glm::vec3 lightPos) {
        _studioEnv.LightPos = lightPos;
    };

    void Shoot();
};

}

#endif // STUDIO_H_INCLUDED
