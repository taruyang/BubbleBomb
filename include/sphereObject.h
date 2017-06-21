#ifndef SPHERE_H_INCLUDED
#define SPHERE_H_INCLUDED

#define GLEW_NO_GLU
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "IGraphicObject.h"
#include "triangleObject.h"
#include "shader.h"

namespace gl
{

using namespace std;

/**
 * @brief   Class to manage and draw a sphere object.
 *          This class inherits the Triangle class because a cube is a composition of multiple triangles.
 *          So, all implementation is same with triangle class except how to draw for the next frame.
 *          In this version, the internal implementation has upgraded to support multiple objects and
 *          Element Buffer Objects.
 *
 */
class SphereObject : public TriangleObject
{

public:
    /**
     * @brief   Constructor of Sphere object
     *
     * @param pShader       Shader Object to be used
     * @param texturePath   Texture file path to wrap this object
     */
    SphereObject(Shader* pShader, const char* texturePath);

    /**
     * @brief   Constructor of Sphere object
     *
     * @param pShader       Shader Object to be used
     * @param objectColor   The color of this object
     */
    SphereObject(Shader* pShader, glm::vec3 objectColor);

    /**
     * @brief   Destructor of Sphere object
     */
    virtual ~SphereObject();

    /**
     * @brief   Draw a frame into a back buffer to display at next frame.
     *
     * @param time      Current time
     * @param PV        Project/View matrix
     * @param studioEnv Studio environment
     * @return  result of method
     */
    virtual bool DrawNextFrame(const double time, glm::mat4 PV, StudioEnv& studioEnv);

    /**
     * @brief   Check this object is intersected with the ray
     * @param rayOrg    Ray origin in world space
     * @param rayDir    Ray direction in world space
     * @return  distance    distance from ray when the object is intersected
     * @return  result of method
     */
    bool IsIntersected(glm::vec3 rayOrg, glm::vec3 rayDir, float *distance);

    /**
     * @brief   Reset Internal status
     * @param isFocused    set current status if this is focused or not
     * @param focusColor   focus color;
     */
    void Reset(StudioEnv& studioEnv);

protected:
    void        DrawContainer();

    glm::mat4   _modelMat;

private:
    /// The location of uniform variables related with light calculation
    GLint _lightPosLoc;
    GLint _viewPosLoc;
    GLint _lightColorLoc;
    GLint _objectColorLoc;
    /// The location of uniform variables related with PVM calculation
    GLint _PVLoc;
    GLint _MLoc;
    GLint _normalMatLoc;
    GLint _seedNum;

    glm::vec3 _movingDistance;
    glm::vec3 _movement;
    double _focusedTime;
    double _startTime;
    /// Get Uniform locations
    void GetUniformLocations();

    /// decide moving distance of this object
    glm::vec3 decideMovingDistance(StudioEnv& studioEnv);
    /// decide random object color
    glm::vec3 decideObjectColor(StudioEnv& studioEnv);
    /// reset values
    void reset(StudioEnv& studioEnv);
    bool _isFirstRendering = true;

    const float DEFAULT_MOVING_DISTANCE = 0.10f;
    const float FOCUSEDTIME = 0.3f;
};
}  /// namespace gl

#endif // SPHERE_H_INCLUDED
